#include "utilities.h"

#include "hittable.h"
#include "hittablelist.h"
#include "triangle.h"

color rayColor(const ray& r, const hittable& world)
{
    hitRecord rec;
    if(world.hit(r, 0, infinity, rec))
    {
        return 0.5f * (rec.normal + color(1,1,1));
    }

    float a = r.direction().y() + 1.0f;
    return (1.0f - a)*(color{1.0,1.0,1.0}) + a*(color{0.5, 0.7, 1.0});
}

int main()
{
    // Calculate output image width and ideal aspect ratio
    constexpr double aspect = 16.0 / 9.0;
    constexpr int imageWidth = 1920;
    constexpr int imageHeight = static_cast<int>(imageWidth / aspect);

    // Make our world!
    hittableList world;
    world.addObject(make_shared<triangle>(point3(-100, 0, 100),
                                            point3(-100, 0, -100),
                                            point3(100, 0, -100)));
    world.addObject(make_shared<triangle>(point3(100, 0, -100),
                                            point3(100, 0, 100),
                                            point3(-100, 0, 100)));
    world.addObject(make_shared<triangle>(point3(0, 0, -10),
                                            point3(-10, 10, -10),
                                            point3(10, 10, -10)));

    // Calculate analytical viewport based on precise viewport aspect
    constexpr double viewportHeight = 2.0;
    constexpr double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
    const point3 cameraPos = {0,5,10};
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

            writeColor(ppm, rayColor({cameraPos, rayDir}, world));
        }
    } 

    ppm.close();
    return 0;
}
