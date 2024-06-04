#include "utilities.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "camera.h"
#include "hittable.h"
#include "hittablelist.h"
#include "triangle.h"
#include "aabb.h"

void addFaces(hittableList& world, const aiMesh* mesh)
{
    hittableList hitMesh;
    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        aiVector3D v0 = mesh->mVertices[face.mIndices[0]];
        aiVector3D v1 = mesh->mVertices[face.mIndices[1]];
        aiVector3D v2 = mesh->mVertices[face.mIndices[2]];

        hitMesh.addObject(make_shared<triangle>(
            point3(v0.x, v0.y, v0.z),
            point3(v1.x, v1.y, v1.z),
            point3(v2.x, v2.y, v2.z)
        ));
    }

    const aiAABB& bounds = mesh->mAABB;
    vec3 min {bounds.mMin.x, bounds.mMin.y, bounds.mMin.z};
    vec3 max {bounds.mMax.x, bounds.mMax.y, bounds.mMax.z};
    world.addObject(make_shared<aabb>(min, max, hitMesh));
}

void processWorld(hittableList& world, aiNode* node, const aiScene* scene)
{
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        addFaces(world, mesh);
    }

    for(int i = 0; i < node->mNumChildren; i++)
    {
        processWorld(world, node->mChildren[i], scene);
    }
}

int main()
{
    Assimp::Importer importer{};
    const aiScene* scene = importer.ReadFile("teapot.obj", aiProcess_Triangulate | aiProcess_FlipUVs 
                                                        | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

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
    processWorld(world, scene->mRootNode, scene);
    // Add a basic large floor
    world.addObject(make_shared<triangle>(point3(-100, 0, 100),
                                            point3(100, 0, -100),
                                            point3(-100, 0, -100)));
    world.addObject(make_shared<triangle>(point3(100, 0, -100),
                                            point3(-100, 0, 100),
                                            point3(100, 0, 100)));
    
    std::cout << "WORLD CREATION COMPLETE\n";

    camera cam;
    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxBounceDepth = 10;

    std::cout << "STARTING RENDER\n";
    cam.render(world); 

    return 0;
}
