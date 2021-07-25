#include "Mesh.hpp"

#include "Filesystem.hpp"

#include <map>
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

static std::string hzglTexturePath(const std::string& parentpath, const std::string& filename)
{
    if (filename.empty())
        return "";

    return parentpath + "/" + filename;
}

static bool hzglHasSmoothingGroup(const tinyobj::shape_t& shape)
{
    for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++)
        if (shape.mesh.smoothing_group_ids[i] > 0)
            return true;

    return false;
}

void hzgl::LoadOBJ(const std::string& filepath, std::vector<MeshInfo>& meshes)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string abspath = GetAbsolutePath(filepath);
    std::string parentpath = GetParentPath(filepath);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, abspath.c_str(), parentpath.c_str()))
    {
        std::cerr << "Failed to load " << filepath << std::endl;
        return;
    }

    if (!warn.empty())
        std::cout << "WARN: " << warn << std::endl;

    if (!err.empty())
        std::cerr << err << std::endl;

    for (unsigned int s = 0; s < shapes.size(); s++)
    {
        const auto& shape = shapes[s];
        size_t vCount = shape.mesh.indices.size();

        if (vCount <= 0)
            continue;

        MeshInfo meshInfo;
        meshInfo.name = shape.name;
        meshInfo.num_vertices = int(vCount);
        meshInfo.positions.resize(vCount * 3, 0);
        meshInfo.normals.resize(vCount * 3, 0);
        meshInfo.texcoords.resize(vCount * 2, 0);

        for (int i = 0; i < meshInfo.num_vertices; i++)
        {
            const auto& idx = shape.mesh.indices[i];

            if (idx.vertex_index >= 0)
            {
                meshInfo.positions[i * 3 + 0] = attrib.vertices[idx.vertex_index * 3 + 0];
                meshInfo.positions[i * 3 + 1] = attrib.vertices[idx.vertex_index * 3 + 1];
                meshInfo.positions[i * 3 + 2] = attrib.vertices[idx.vertex_index * 3 + 2];
            }

            if (idx.normal_index >= 0)
            {
                meshInfo.normals[i * 3 + 0] = attrib.normals[idx.normal_index * 3 + 0];
                meshInfo.normals[i * 3 + 1] = attrib.normals[idx.normal_index * 3 + 1];
                meshInfo.normals[i * 3 + 2] = attrib.normals[idx.normal_index * 3 + 2];
            }

            if (idx.texcoord_index >= 0)
            {
                meshInfo.texcoords[i * 2 + 0] = attrib.texcoords[idx.texcoord_index * 2 + 0];
                meshInfo.texcoords[i * 2 + 1] = attrib.texcoords[idx.texcoord_index * 2 + 1];
            }
        }

        // TEMP: use normal mapping by default
        meshInfo.shading_mode = HZGL_NORMAL_MAPPING;

        // Do nothing if no valid material available
        if (!shape.mesh.material_ids.empty() && !materials.empty())
        {
            // One material for the entire shape
            int matID = shape.mesh.material_ids[0];

            // matID == -1: no material for the shape
            if (matID < 0)
                continue;

            const auto& mat = materials[matID];

            meshInfo.texpath["albedo"] = hzglTexturePath(parentpath, mat.diffuse_texname);
            meshInfo.texpath["ambient"] = hzglTexturePath(parentpath, mat.ambient_texname);
            meshInfo.texpath["specular"] = hzglTexturePath(parentpath, mat.specular_texname);

            meshInfo.texpath["normal"] = hzglTexturePath(parentpath, mat.normal_texname);
            meshInfo.texpath["bump"] = hzglTexturePath(parentpath, mat.bump_texname);
            meshInfo.texpath["displacement"] = hzglTexturePath(parentpath, mat.displacement_texname);

            meshInfo.texpath["roughness"] = hzglTexturePath(parentpath, mat.roughness_texname);
            meshInfo.texpath["metallic"] = hzglTexturePath(parentpath, mat.metallic_texname);
            meshInfo.texpath["sheen"] = hzglTexturePath(parentpath, mat.sheen_texname);
            meshInfo.texpath["emissive"] = hzglTexturePath(parentpath, mat.emissive_texname);

            meshInfo.texpath["specular_highlight"] = hzglTexturePath(parentpath, mat.specular_highlight_texname);
            meshInfo.texpath["reflection"] = hzglTexturePath(parentpath, mat.reflection_texname);

            // TODO:
            //   - alpha
            //   - roughness
            //   - metallic
            //   - sheen
            //   - anisotropy_*
            //   - clearcoat_*
        }

        meshes.push_back(meshInfo);
    }
}