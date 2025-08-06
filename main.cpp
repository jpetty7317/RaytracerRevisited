#include "utilities.h"
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>
#include "camera.h"
#include "model.h"
#include "triangle.h"
#include "aabb.h"
#include "tlas.h"
#include "material.h"
#include "rtw_stb_image.h"
#include <thread>
#include <string>

void addFaces(std::vector<shared_ptr<model>>& modelList, const aiMesh* mesh, const shared_ptr<material> mat)
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

        aiVector3D n0 = mesh->mNormals[face.mIndices[0]];
        aiVector3D n1 = mesh->mNormals[face.mIndices[1]];
        aiVector3D n2 = mesh->mNormals[face.mIndices[2]];

        vec3 uv0{};
        vec3 uv1{};
        vec3 uv2{};
        if (mesh->HasTextureCoords(0)) {
            float x = mesh->mTextureCoords[0][face.mIndices[0]].x;
            float y = mesh->mTextureCoords[0][face.mIndices[0]].y;
            uv0 = vec3{x, y, 0.0f};

            x = mesh->mTextureCoords[0][face.mIndices[1]].x;
            y = mesh->mTextureCoords[0][face.mIndices[1]].y;
            uv1 = vec3{x, y, 0.0f};

            x = mesh->mTextureCoords[0][face.mIndices[2]].x;
            y = mesh->mTextureCoords[0][face.mIndices[2]].y;
            uv2 = vec3{x, y, 0.0f};
        }


        hitMesh->addTriangle(make_shared<triangle>(
            point3(v0.x, v0.y, v0.z),
            point3(v1.x, v1.y, v1.z),
            point3(v2.x, v2.y, v2.z),
            vec3{n0.x, n0.y, n0.z},
            vec3{n1.x, n1.y, n1.z},
            vec3{n2.x, n2.y, n2.z},
            uv0,
            uv1,
            uv2,
            mat
        ));
    }

    hitMesh->mbvh = { &hitMesh->triangles, (int)hitMesh->triangles.size() };

    modelList.push_back(hitMesh);
}

void buildModelList(std::vector<shared_ptr<model>>& modelList, std::vector<shared_ptr<material>>& matList, aiNode* node, const aiScene* scene)
{
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        shared_ptr<material> mat = matList[mesh->mMaterialIndex];
        addFaces(modelList, mesh, mat);
    }

    for(int i = 0; i < node->mNumChildren; i++)
    {
        buildModelList(modelList, matList, node->mChildren[i], scene);
    }
}

void buildMaterialList(std::vector<shared_ptr<material>>& materialList, const aiScene* scene, const std::string& folder) {
    for(int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        aiString texturePath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
        std::string texName {texturePath.data};
        shared_ptr<texture> matTex = make_shared<texture>((folder + texName).c_str());
        shared_ptr<lambertian> newMat =make_shared<lambertian>(color{1,1,1}, matTex);
        materialList.push_back(newMat);
    }
}

int main()
{
    Assimp::Importer importer{};
    const aiScene* scene = importer.ReadFile("sponza\\sponza.obj", aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                        | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

    //const aiScene* scene = importer.ReadFile("teapot.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals
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
    std::vector<shared_ptr<material>> globalMaterialList;
    buildMaterialList(globalMaterialList, scene, "sponza\\");
    buildModelList(globalModelList, globalMaterialList, scene->mRootNode, scene);

    /*point3 p1 = point3{-600, 1500, 100};
    point3 p0 = point3{-800, 1500, -100};
    point3 p2 = point3{-800, 1500, 100};
    point3 p3 = point3{-600, 1500, -100};
    shared_ptr<model> triLight = make_shared<model>(
        point3{-801, 1499, -101},
        point3{-599, 1501, 101}
    );
    triLight->addTriangle(make_shared<triangle>(p0, p1, p2, lightMat));
    triLight->addTriangle(make_shared<triangle>(p0, p3, p1, lightMat));
    triLight->mbvh = { &triLight->triangles, (int)triLight->triangles.size() };
    globalModelList.push_back(triLight);*/

    tlas t {&globalModelList, (int)globalModelList.size()};

    camera cam;
    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 1280;
    cam.samplesPerPixel = 1;
    cam.maxBounceDepth = 5;
    cam.vfov = 90;
    cam.lookFrom = point3{0.0, 530.0, 0.0};
    cam.lookAt = point3{-3.0, 530.0, 0.0};
    //cam.lookFrom = point3{0.0, 3.0, 3.0};
    //cam.lookAt = point3{0.0, 1.0, 0.0};
    cam.vUp = vec3{0,1,0};

    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    std::cout << "STARTING RENDER\n";
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    cam.render(t); 
    std::cout << "TIME TO RENDER: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << '\n';
    return 0;
}
