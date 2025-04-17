#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <thread>

#include "utilities.h"
#include "tlas.h"

class camera;

void renderRow(int tx, int ty, int nx, int ny, int ns, int maxBounceDepth, tlas& tlas, const camera& cam, std::vector<color>* output);

class camera
{
public:
    double aspectRatio = 1.0;    // Ratio of image width / height
    int imageWidth = 100;        // Rendered image width in pixels
    int samplesPerPixel = 10;    // Count of random samples for each pixel
    int maxBounceDepth = 10;     // Maximum number of bounces per ray
    double vfov = 90;            // Vertical view angle (field of view)
    point3 lookFrom = point3{0,0,0};
    point3 lookAt = point3{0,0,-1};
    vec3 vUp = vec3{0,1,0};

    double getInvPixelSamples() const { return pixelSamplesInv; }

    void render(tlas& t)
    {
        initialize();

        std::vector<std::thread> threadPool;
        std::vector<color> output(imageWidth * imageHeight);

        int tX = (int)std::ceil((float)imageWidth / 16.0f);
        int tY = (int)std::ceil((float)imageHeight / 16.0f);
        int numTiles = tX * tY;
        for(int tile = 0; tile < numTiles; tile++)
        {
            int x = tile % (int)tX;
            int y = tile / (int)tX;

            threadPool.emplace_back(renderRow, x, y, imageWidth, imageHeight, samplesPerPixel, maxBounceDepth, std::ref(t), *this, &output);
            //renderRow(x, y, imageWidth, imageHeight, samplesPerPixel, maxBounceDepth, t, *this, &output);
        }

        for(auto& thread : threadPool)
        {
            thread.join();
        }

        std::ofstream ppm;
        ppm.open("output.ppm");

        ppm << "P3\n" << imageWidth << " " << imageHeight << " \n255\n";

        for(int y = 0; y < imageHeight; y++)
        {
            for(int x = 0; x < imageWidth; x++)
            {
                writeColor(ppm, output[x + y * imageWidth]);
            }
        } 

        ppm.close();
    }

    ray getRay(int i, int j) const
    {
        vec3 offset = sampleSquare();
        vec3 pixelSample = pixel00Pos + ((i + offset.x()) * pixelDeltaU) + ((j + offset.y()) * pixelDeltaV);
        //vec3 pixelSample = pixel00Pos + (i * pixelDeltaU) + (j * pixelDeltaV);

        return ray{cameraPos, pixelSample - cameraPos};
    }

    color rayColor(ray& r, int depth, tlas& t) const 
    {
        if(depth <= 0)
            return vec3{0,0,0};

        t.hit(r);

        if(r.t != infinity)
        {
            vec3 direction = r.normal + randomVectorOnHemisphere(r.normal);
            r = ray{r.at(r.t), direction};
            return 0.5f * rayColor(r, depth - 1, t);
        }

        float a = r.direction().y() + 1.0f;
        return (1.0f - a)*(color{1.0,1.0,1.0}) + a*(color{0.5, 0.7, 1.0});
    }

private:
    int imageHeight;    // Rendered image height
    double pixelSamplesInv; // Inverse of pixel samples to scale result
    point3 cameraPos;   // Camera position
    point3 pixel00Pos;  // World pos of pixel 0,0
    vec3 pixelDeltaU;   // Offset to center of pixel to the right
    vec3 pixelDeltaV;   // Offset to center of pixel below
    
    // Orthonormal basis vectors for orienting the camera arbitrarily
    vec3 u;
    vec3 v;
    vec3 w;

    void initialize()
    {
        imageHeight = int(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesInv = 1.0 / samplesPerPixel;

        cameraPos = lookFrom;

        // Calculate analytical viewport based on precise viewport aspect
        const float focalLength = (lookFrom - lookAt).length();
        const double theta = degreesToRadians(vfov);
        const double h = tan(theta/2.0);
        const double viewportHeight = 2 * h * focalLength;
        const double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame
        w = (lookFrom - lookAt).normalize();
        u = cross(vUp, w).normalize();
        v = cross(w, u);

        vec3 viewPortU {float(viewportWidth) * u};
        vec3 viewPortV {float(viewportHeight) * -v};

        pixelDeltaU = viewPortU / imageWidth;
        pixelDeltaV = viewPortV / imageHeight;

        point3 viewportUpperLeft = cameraPos - (focalLength * w) - (viewPortU / 2.0) - (viewPortV / 2.0);
        pixel00Pos = viewportUpperLeft + ((pixelDeltaU + pixelDeltaV) * 0.5);
    }

    vec3 sampleSquare() const
    {
        return vec3{ randGen<float>() - 0.5f, randGen<float>() - 0.5f, 0.0f };
    }
};

void renderRow(int tx, int ty, int nx, int ny, int ns, int maxBounceDepth, tlas& t, const camera& cam, std::vector<color>* output)
{
    for(int v = 0; v < 16; v++)
    {
        for(int u = 0; u < 16; u++)
        {
            float x = (tx * 16 + u);
            float y = (ty * 16 + v);

            if(x >= nx || y >= ny)
                continue;

            vec3 col {0,0,0};
            for(int s = 0; s < ns; s++)
            {
                ray r = cam.getRay(x, y);
                col += cam.rayColor(r, maxBounceDepth, t);
            }

            col *= cam.getInvPixelSamples();

            (*output)[y * nx + x] = col;
        }
    }
}
#endif
