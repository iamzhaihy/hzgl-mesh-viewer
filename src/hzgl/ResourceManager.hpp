#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <string>
#include <unordered_map>

#include <glad/glad.h>

namespace hzgl
{
    typedef struct
    {
        // Metadata
        std::string name;
        int num_vertices;

        // OpenGL related
        GLuint VAO;
        ShadingMode shading_mode;
        std::unordered_map<std::string, GLuint> texture;
    } RenderObject;

    typedef struct
    {
        GLuint id;
        std::string filepath;
    } ShaderInfo;

    typedef struct 
    {
        GLuint id;
        std::string name;
        std::vector<ShaderStage> stages;
        // TODO: a list of uniform variables
    } ProgramInfo;

    typedef struct
    {
        int start_index;
        int num_meshes;
        std::string filepath;
    } RenderObjectInfo;

    class ResourceManager
    {
    private:
        std::vector<GLuint> _usedVAOs;                // OpenGL handle
        std::vector<GLuint> _usedVBOs;                // OpenGL handle
        std::vector<std::string> _loadedMeshes;       // filepath of the 3D model
        std::vector<std::string> _loadedShaders;      // filepath of the shader source
        std::vector<std::string> _loadedTextures;     // filepath of the texture image
        std::vector<std::string> _loadedPrograms;     // unique "name" for the program

        std::unordered_map<std::string, ShaderInfo> _shaderInfo;
        std::unordered_map<std::string, ProgramInfo> _programInfo;
        std::unordered_map<std::string, TextureInfo> _textureInfo;
        std::unordered_map<std::string, RenderObjectInfo> _renderObjectInfo;

    public:
        ResourceManager();
        ~ResourceManager();

        // manual memory release
        void ReleaseAll();

        // loading assets from files
        void LoadMesh(const std::string& filepath, std::vector<RenderObject>& objects);
        GLuint LoadTexture(const std::string& filepath, GLenum type);
        GLuint LoadShader(const std::string& filepath, GLenum shaderType);
        GLuint LoadShaderProgram(std::vector<ShaderStage> shaders, const char* name = nullptr);

        // public getters
        std::vector<std::string> GetLoadedMeshesNames();
        std::vector<std::string> GetLoadedTextureNames();
        std::vector<std::string> GetLoadedShaderProgramNames();

        GLuint GetProgramID(int index);
        GLuint GetProgramID(const std::string& name);

        GLuint GetTextureID(int index);
        GLuint GetTextureID(const std::string& name);

        int GetTextureWidth(int index);
        int GetTextureWidth(const std::string& name);
        int GetTextureHeight(int index);
        int GetTextureHeight(const std::string& name);
    };
} // namespace hzgl