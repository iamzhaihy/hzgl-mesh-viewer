#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace hzgl
{
    typedef enum
    {
        HZGL_FLAT,
        HZGL_PHONG,
        HZGL_NORMAL_MAPPING,
        HZGL_PBR
    } ShadingMode;

    typedef struct
    {
        // Metadata
        std::string name;

        // Geometry
        int num_vertices;
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> texcoords;
        std::vector<unsigned> indices;

        // Material
        ShadingMode shading_mode;
        std::unordered_map<std::string, std::string> texpath;
    } MeshInfo;

    std::string ShadingModeName(ShadingMode mode);
    void LoadMeshesFromFile(const std::string &filepath, std::vector<MeshInfo> &meshes);
} // namespace hzgl
