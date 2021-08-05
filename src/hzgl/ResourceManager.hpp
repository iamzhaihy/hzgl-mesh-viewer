#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <string>
#include <unordered_map>

#include <glad/glad.h>

namespace hzgl
{
    typedef struct _RenderShape
    {
        // Metadata
        std::string name;
        int num_vertices = 0;
        bool has_normals = false;
        bool has_texcoords = false;
        bool has_textures = false;

        // OpenGL related
        GLuint VAO = 0;
        ShadingMode shading_mode;
        std::unordered_map<std::string, GLuint> texture;
    } RenderShape;

    typedef struct _RenderObject
    {
        // Metadata
        std::string path = "";
        int num_shapes = 0;

        std::vector<RenderShape> shapes;
    } RenderObject;

    typedef struct _ShaderInfo
    {
        GLuint id = 0;
        std::string filepath = "";
    } ShaderInfo;

    typedef struct _ProgramInfo
    {
        GLuint id = 0;
        std::string name = "";
        std::vector<ShaderStage> stages;
        // TODO: a list of uniform variables
    } ProgramInfo;

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
        std::unordered_map<std::string, RenderObject> _renderObjects;

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