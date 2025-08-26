#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <thread>

#include "utilities.h"
#include "tlas.h"

class camera;

void renderRow(int tx, int ty, int nx, int ny, int ns, int maxBounceDepth, tlas& tlas, const camera& cam, std::vector<glm::vec3>* output);

class camera
{
public:
    double aspectRatio = 1.0;    // Ratio of image width / height
    int imageWidth = 100;        // Rendered image width in pixels
    int samplesPerPixel = 10;    // Count of random samples for each pixel
    int maxBounceDepth = 10;     // Maximum number of bounces per ray
    double vfov = 90;            // Vertical view angle (field of view)
    glm::vec3 lookFrom = glm::vec3{0,0,0};
    glm::vec3 lookAt = glm::vec3{0,0,-1};

    double getInvPixelSamples() const { return pixelSamplesInv; }

    void render(tlas& t)
    {
        initialize();

        std::vector<std::thread> threadPool;
        std::vector<glm::vec3> output(imageWidth * imageHeight);

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

    ray getRay(int i, int j, uint32_t& seed) const
    {
        glm::vec3 offset = sampleSquare(seed);
        glm::vec3 pixelSample = pixel00Pos + ((i + offset.x) * pixelDeltaU) + ((j + offset.y) * pixelDeltaV);
        //vec3 pixelSample = pixel00Pos + (i * pixelDeltaU) + (j * pixelDeltaV);

        return ray{cameraPos, pixelSample - cameraPos};
    }

    glm::vec3 lightDir {-0.6f, -0.7f, 0.1f};

    glm::vec3 rayColor(ray& r, int depth, tlas& t, uint32_t& seed) const
    {
        if(depth <= 0)
            return glm::vec3{0,0,0};

        t.hit(r);

        if(r.t == infinity)
        {
            return glm::vec3 {0.5,0.58,0.93};
        }

        ray shadow {r.at(r.t), -lightDir};
        t.hit(shadow);
        glm::vec3 attenuation = glm::vec3{4,4,4};
        if (shadow.t != infinity) {
            attenuation = glm::vec3{0,0,0};
        }

        glm::vec3 matColor;
        glm::vec3 scatteredDir;
        glm::vec3 normal {r.normal};
        r.mat->scatter(r.direction(), r.normal, r.uv, matColor, scatteredDir, seed);
        r = ray{r.at(r.t), scatteredDir};
        return attenuation * matColor * std::max(0.0f, dot(normal, -lightDir)) + (matColor * rayColor(r, depth - 1, t, seed));
    }

private:
    int imageHeight;    // Rendered image height
    double pixelSamplesInv; // Inverse of pixel samples to scale result
    glm::vec3 cameraPos;   // Camera position
    glm::vec3 pixel00Pos;  // World pos of pixel 0,0
    glm::vec3 pixelDeltaU;   // Offset to center of pixel to the right
    glm::vec3 pixelDeltaV;   // Offset to center of pixel below
    
    // Orthonormal basis vectors for orienting the camera arbitrarily
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;

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
        w = glm::normalize(lookFrom - lookAt);
        u = glm::normalize(glm::cross(vec3::up(), w));
        v = glm::cross(w, u);

        glm::vec3 viewPortU {float(viewportWidth) * u};
        glm::vec3 viewPortV {float(viewportHeight) * -v};

        pixelDeltaU = viewPortU / imageWidth;
        pixelDeltaV = viewPortV / imageHeight;

        glm::vec3 viewportUpperLeft = cameraPos - (focalLength * w) - (viewPortU / 2.0) - (viewPortV / 2.0);
        pixel00Pos = viewportUpperLeft + ((pixelDeltaU + pixelDeltaV) * 0.5);
    }

    glm::vec3 sampleSquare(uint32_t& seed) const
    {
        return glm::vec3{ randGen<float>(seed) - 0.5f, randGen<float>(seed) - 0.5f, 0.0f };
    }
};

void renderRow(int tx, int ty, int nx, int ny, int ns, int maxBounceDepth, tlas& t, const camera& cam, std::vector<glm::vec3>* output)
{
    for(int v = 0; v < 16; v++)
    {
        for(int u = 0; u < 16; u++)
        {
            float x = (tx * 16 + u);
            float y = (ty * 16 + v);

            if(x >= nx || y >= ny)
                continue;

            uint32_t seed = x + y * nx;
            glm::vec3 col {0,0,0};
            for(int s = 0; s < ns; s++)
            {
                seed += s;
                ray r = cam.getRay(x, y, seed);
                col += cam.rayColor(r, maxBounceDepth, t, seed);
            }

            col *= cam.getInvPixelSamples();

            (*output)[y * nx + x] = col;
        }
    }
}
#endif
