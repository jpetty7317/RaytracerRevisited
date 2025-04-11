#include "utilities.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>
#include "camera.h"
#include "hittable.h"
#include "model.h"
#include "triangle.h"
#include "aabb.h"
#include <thread>

void addFaces(std::vector<shared_ptr<model>>& modelList, const aiMesh* mesh)
{
    const aiAABB& bounds = mesh->mAABB;
    vec3 min {bounds.mMin.x, bounds.mMin.y, bounds.mMin.z};
    vec3 max {bounds.mMax.x, bounds.mMax.y, bounds.mMax.z};

    shared_ptr<model> hitMesh = make_shared<model>(min, max);
    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        aiVector3D v0 = mesh->mVertices[face.mIndices[0]];
        aiVector3D v1 = mesh->mVertices[face.mIndices[1]];
        aiVector3D v2 = mesh->mVertices[face.mIndices[2]];

        hitMesh->addTriangle(make_shared<triangle>(
            point3(v0.x, v0.y, v0.z),
            point3(v1.x, v1.y, v1.z),
            point3(v2.x, v2.y, v2.z)
        ));
    }

    hitMesh->mbvh = { &hitMesh->triangles, hitMesh->triangles.size() };

    modelList.push_back(hitMesh);
}

void buildModelList(std::vector<shared_ptr<model>>& modelList, aiNode* node, const aiScene* scene)
{
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        addFaces(modelList, mesh);
    }

    for(int i = 0; i < node->mNumChildren; i++)
    {
        buildModelList(modelList, node->mChildren[i], scene);
    }
}

int main()
{
    Assimp::Importer importer{};
    const aiScene* scene = importer.ReadFile("sponza\\sponza.obj", aiProcess_Triangulate | aiProcess_FlipUVs 
                                                        | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

    //const aiScene* scene = importer.ReadFile("teapot.obj", aiProcess_Triangulate | aiProcess_FlipUVs
    //                                                    | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return 0;
    }
    else
    {
        std::cout << "HEY WE IMPORTED THE THING!!! " << scene->mRootNode->mName.C_Str() << "\n";
    }

    std::vector<shared_ptr<model>> globalModelList;
    buildModelList(globalModelList, scene->mRootNode, scene);

    camera cam;
    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;//1920;
    cam.samplesPerPixel = 1;//10;
    cam.maxBounceDepth = 1;//50;
    cam.vfov = 90;
    cam.lookFrom = point3{0.0, 530.0, 0.0};//point3{0.0, 1.7, 5.0};//
    cam.lookAt = point3{-3.0, 530.0, 0.0};//point3{0.0, 1.7, 0.0};//
    cam.vUp = vec3{0,1,0};

    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    std::cout << "STARTING RENDER\n";
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    cam.render(globalModelList); 
    std::cout << "TIME TO RENDER: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << '\n';
    return 0;
}
