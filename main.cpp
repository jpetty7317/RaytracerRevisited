#include <iostream>
#include <fstream>

#include "vec3.h"
#include "color.h"
#include "ray.h"

struct triangle
{
    vec3 p0{};
    vec3 p1{};
    vec3 p2{};
};

bool hitTriangle(const ray& r, const triangle& tri)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    //Get tri edges
    vec3 e1 = tri.p1 - tri.p0;
    vec3 e2 = tri.p2 - tri.p0;
    vec3 rCrossE2 = cross(r.direction(), e2);
    float det = dot(e1, rCrossE2);

    if(det > -epsilon && det < epsilon)
        return false;

    float invDet = 1.0f / det;
    vec3 s = r.origin() - tri.p0;
    float u = invDet * dot(s, rCrossE2);

    if(u < 0.0f || u > 1.0f)
        return false;

    vec3 sCrossE1 = cross(s, e1);
    float v = invDet * dot(r.direction(), sCrossE1);

    if(v < 0.0f || u + v > 1.0f)
        return false;

    float t = invDet * dot(e2, sCrossE1);

    if(t > epsilon)
    {
        // intersectionPoint = r.origin() + r.direction() * t;
        return true;
    }
    else
    {
        return false;
    }
}

color rayColor(const ray& r)
{
    if(hitTriangle(r, triangle{{-0.5, 0.5, -1.0},{0,-0.5,-1.0},{0.5, 0.5, -1.0}}))
        return color {1,0,0};

    float a = r.direction().y() + 1.0f;
    return (1.0f - a)*(color{1.0,1.0,1.0}) + a*(color{0.5, 0.7, 1.0});
}

int main()
{
    // Calculate output image width and ideal aspect ratio
    constexpr double aspect = 16.0 / 9.0;
    constexpr int imageWidth = 1920;
    constexpr int imageHeight = static_cast<int>(imageWidth / aspect);

    // Calculate analytical viewport based on precise viewport aspect
    constexpr double viewportHeight = 2.0;
    constexpr double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
    const point3 cameraPos = {0,0,0};
    const float focalLength = 1.0f;

    vec3 viewPortU {viewportWidth, 0.0, 0.0};
    vec3 viewPortV {0.0, -viewportHeight, 0.0};

    vec3 pixelDeltaU {viewPortU / imageWidth};
    vec3 pixelDeltaV {viewPortV / imageHeight};

    point3 viewportUpperLeft = cameraPos - vec3(0,0,focalLength) - (viewPortU / 2.0) - (viewPortV / 2.0);
    point3 pixel00Pos = viewportUpperLeft + ((pixelDeltaU + pixelDeltaV) * 0.5);

    std::ofstream ppm;
    ppm.open("output.ppm");

    ppm << "P3\n" << imageWidth << " " << imageHeight << " \n255\n";

    for(int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            // Show Progress
            //std::cout << ((i * width + j) / (double)(width * height)) * 100 << "% done\n" << std::flush;
            point3 pixelCenter = pixel00Pos + (j * pixelDeltaU) + (i * pixelDeltaV);
            vec3 rayDir = (pixelCenter - cameraPos);

            writeColor(ppm, rayColor({cameraPos, rayDir}));
        }
    } 

    ppm.close();
    return 0;
}
