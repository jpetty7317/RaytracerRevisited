#include "utilities.h"

#include "camera.h"
#include "hittable.h"
#include "hittablelist.h"
#include "triangle.h"

int main()
{
    // Make our world!
    hittableList world;
    world.addObject(make_shared<triangle>(point3(-100, 0, 100),
                                            point3(100, 0, -100),
                                            point3(-100, 0, -100)));
    world.addObject(make_shared<triangle>(point3(100, 0, -100),
                                            point3(-100, 0, 100),
                                            point3(100, 0, 100)));
    world.addObject(make_shared<triangle>(point3(0, 10, -10),
                                            point3(-10, 0, -10),
                                            point3(10, 0, -10)));

    camera cam;
    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 1920;
    cam.samplesPerPixel = 10;
    cam.maxBounceDepth = 50;
    cam.render(world); 

    return 0;
}
