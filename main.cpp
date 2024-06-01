#include "utilities.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "camera.h"
#include "hittable.h"
#include "hittablelist.h"
#include "triangle.h"

int main()
{
    Assimp::Importer importer{};
    const aiScene* scene = importer.ReadFile("teapot.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return 0;
    }
    else
    {
        std::cout << "HEY WE IMPORTED THE THING!!! " << scene->mRootNode->mName.C_Str() << "\n";
    }

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
