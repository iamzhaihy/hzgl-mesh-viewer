#include "Mesh.hpp"

#include "Filesystem.hpp"

#include <map>
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static std::string hzglTexturePath(const std::string &parentpath, const std::string &filename)
{
    if (filename.empty())
        return "";

    return parentpath + "/" + filename;
}

static std::string hzglTextureTypeName(aiTextureType type)
{
#define CASE(key, ret)  \
    case key:           \
    {                   \
        typeName = ret; \
        break;          \
    }

    std::string typeName;

    switch (type)
    {
        CASE(aiTextureType_NONE, "none")
        CASE(aiTextureType_DIFFUSE, "diffuse")
        CASE(aiTextureType_SPECULAR, "specular")
        CASE(aiTextureType_AMBIENT, "ambient")
        CASE(aiTextureType_EMISSIVE, "emissive")
        CASE(aiTextureType_HEIGHT, "height")
        CASE(aiTextureType_NORMALS, "normals")
        CASE(aiTextureType_SHININESS, "shininess")
        CASE(aiTextureType_OPACITY, "opacity")
        CASE(aiTextureType_DISPLACEMENT, "displacement")
        CASE(aiTextureType_LIGHTMAP, "lightmap")
        CASE(aiTextureType_REFLECTION, "reflection")
        CASE(aiTextureType_BASE_COLOR, "base")
        CASE(aiTextureType_NORMAL_CAMERA, "normal")
        CASE(aiTextureType_EMISSION_COLOR, "emission")
        CASE(aiTextureType_METALNESS, "metalness")
        CASE(aiTextureType_DIFFUSE_ROUGHNESS, "diffuse")
        CASE(aiTextureType_AMBIENT_OCCLUSION, "ambient")
        CASE(aiTextureType_SHEEN, "sheen")
        CASE(aiTextureType_CLEARCOAT, "clearcoat")
        CASE(aiTextureType_TRANSMISSION, "transmission")
        CASE(aiTextureType_UNKNOWN, "unknown")
    default:
        break;
    }

#undef CASE

    return typeName;
}

static void hzglProcessAiMesh(const aiScene *scene, const aiMesh *mesh, std::vector<hzgl::MeshInfo> &loadedShapes, std::string parentpath = "")
{
    hzgl::MeshInfo meshInfo;
    meshInfo.name = mesh->mName.C_Str();
    meshInfo.num_vertices = mesh->mNumVertices;
    meshInfo.shading_mode = hzgl::HZGL_NORMAL_MAPPING;

    // allocate space for geometry data
    meshInfo.normals.resize(meshInfo.num_vertices * 3, 0);
    meshInfo.positions.resize(meshInfo.num_vertices * 3, 0);
    meshInfo.texcoords.resize(meshInfo.num_vertices * 2, 0);
    meshInfo.indices.resize(mesh->mNumFaces * 3, 0);

    for (unsigned i = 0; i < mesh->mNumVertices; i++)
    {
        // retrieve positions (guaranteed to exist)
        meshInfo.positions[i * 3 + 0] = mesh->mVertices[i].x;
        meshInfo.positions[i * 3 + 1] = mesh->mVertices[i].y;
        meshInfo.positions[i * 3 + 2] = mesh->mVertices[i].z;

        // retrieve vertex normals (if present)
        if (mesh->HasNormals())
        {
            meshInfo.normals[i * 3 + 0] = mesh->mNormals[i].x;
            meshInfo.normals[i * 3 + 1] = mesh->mNormals[i].y;
            meshInfo.normals[i * 3 + 2] = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0])
        {
            meshInfo.texcoords[i * 2 + 0] = mesh->mTextureCoords[0][i].x;
            meshInfo.texcoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
        }
    }

    for (unsigned f = 0; f < mesh->mNumFaces; f++)
    {
        const auto &face = mesh->mFaces[f];

        meshInfo.indices[f * 3 + 0] = face.mIndices[0];
        meshInfo.indices[f * 3 + 1] = face.mIndices[1];
        meshInfo.indices[f * 3 + 2] = face.mIndices[2];
    }

    const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    for (unsigned t = 0; t <= 20; t++)
    {
        aiTextureType type = (aiTextureType)t;
        std::string typeName = hzglTextureTypeName(type);

        for (unsigned i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString texPath;
            material->GetTexture(type, i, &texPath);

            std::string key = (i > 0) ? typeName + std::to_string(i) : typeName;
            meshInfo.texpath[key] = hzglTexturePath(parentpath, texPath.C_Str());
        }
    }

    loadedShapes.push_back(meshInfo);
}

static void hzglProcessAiNode(const aiScene *scene, const aiNode *node, std::vector<hzgl::MeshInfo> &loadedShapes, const aiMatrix4x4 &accTransform = aiMatrix4x4(), std::string parentpath = "")
{
    aiMatrix4x4 transform = node->mTransformation * accTransform;

    // process the meshes referred by the current node
    for (unsigned m = 0; m < node->mNumMeshes; m++)
    {
        unsigned mIndex = node->mMeshes[m];
        hzglProcessAiMesh(scene, scene->mMeshes[mIndex], loadedShapes, parentpath);
    }

    // recursively process all the children nodes
    for (unsigned c = 0; c < node->mNumChildren; c++)
        hzglProcessAiNode(scene, node->mChildren[c], loadedShapes, transform, parentpath);
}

std::string hzgl::ShadingModeName(ShadingMode mode)
{
    std::string shadingMode;

    switch (mode)
    {
    case HZGL_FLAT:
        shadingMode = "Flat Shading";
        break;
    case HZGL_PHONG:
        shadingMode = "Phong Shading";
        break;
    case HZGL_NORMAL_MAPPING:
        shadingMode = "Normal Mapping";
        break;
    case HZGL_PBR:
        shadingMode = "Physically Based Rendering";
        break;
    default:
        shadingMode = "Unknown type";
        break;
    }

    return shadingMode;
}

void hzgl::LoadMeshesFromFile(const std::string &filepath, std::vector<hzgl::MeshInfo> &loadedShapes)
{
    std::string abspath = GetAbsolutePath(filepath);
    std::string parentpath = GetParentPath(filepath);

    Assimp::Importer importer;

    auto flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords;
    const aiScene *scene = importer.ReadFile(abspath, flags);

    // If the import failed, report it
    if (scene == nullptr || !scene->HasMeshes() || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        return;
    }

    aiMatrix4x4 accTransform;
    hzglProcessAiNode(scene, scene->mRootNode, loadedShapes, accTransform, parentpath);
}