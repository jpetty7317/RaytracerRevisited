#ifndef CAMERA_H
#define CAMERA_H

#include "utilities.h"
#include "hittable.h"

class camera
{
public:
    double aspectRatio = 1.0;    // Ratio of image width / height
    int imageWidth = 100;        // Rendered image width in pixels
    int samplesPerPixel = 10;    // Count of random samples for each pixel
    int maxBounceDepth = 10;     // Maximum number of bounces per ray

    void render(const hittable& world)
    {
        initialize();

        std::ofstream ppm;
        ppm.open("output.ppm");

        ppm << "P3\n" << imageWidth << " " << imageHeight << " \n255\n";

        for(int j = 0; j < imageHeight; j++)
        {
            for(int i = 0; i < imageWidth; i++)
            {
                color pixelCol {0,0,0};
                for(int s = 0; s < samplesPerPixel; s++)
                {
                    pixelCol += rayColor(getRay(i, j), maxBounceDepth, world);
                }

                writeColor(ppm, pixelCol * pixelSamplesInv);
            }
        } 

        ppm.close();
    }

private:
    int imageHeight;    // Rendered image height
    double pixelSamplesInv; // Inverse of pixel samples to scale result
    point3 cameraPos;   // Camera position
    point3 pixel00Pos;  // World pos of pixel 0,0
    vec3 pixelDeltaU;   // Offset to center of pixel to the right
    vec3 pixelDeltaV;   // Offset to center of pixel below

    void initialize()
    {
        imageHeight = int(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesInv = 1.0 / samplesPerPixel;

        cameraPos = {0,5,10};

        // Calculate analytical viewport based on precise viewport aspect
        const double viewportHeight = 2.0;
        const double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
        const float focalLength = 1.0f;

        vec3 viewPortU {float(viewportWidth), 0.0, 0.0};
        vec3 viewPortV {0.0, float(-viewportHeight), 0.0};

        pixelDeltaU = viewPortU / imageWidth;
        pixelDeltaV = viewPortV / imageHeight;

        point3 viewportUpperLeft = cameraPos - vec3(0,0,focalLength) - (viewPortU / 2.0) - (viewPortV / 2.0);
        pixel00Pos = viewportUpperLeft + ((pixelDeltaU + pixelDeltaV) * 0.5);
    }

    ray getRay(int i, int j) const
    {
        vec3 offset = sampleSquare();
        vec3 pixelSample = pixel00Pos + ((i + offset.x()) * pixelDeltaU) + ((j + offset.y()) * pixelDeltaV);

        return ray{cameraPos, pixelSample - cameraPos};
    }

    vec3 sampleSquare() const
    {
        return vec3{ randGen<float>() - 0.5f, randGen<float>() - 0.5f, 0.0f };
    }

    color rayColor(const ray& r, int depth, const hittable& world) const 
    {
        if(depth <= 0)
            return vec3{0,0,0};

        hitRecord rec;
        if(world.hit(r, interval{0.001, infinity}, rec))
        {
            vec3 direction = rec.normal + randomVectorOnHemisphere(rec.normal);
            return 0.5f * rayColor(ray{rec.point, direction}, depth - 1, world);
        }

        float a = r.direction().y() + 1.0f;
        return (1.0f - a)*(color{1.0,1.0,1.0}) + a*(color{0.5, 0.7, 1.0});
    }
};

#endif
