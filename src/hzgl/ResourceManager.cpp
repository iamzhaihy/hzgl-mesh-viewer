#include "ResourceManager.hpp"

#include "Filesystem.hpp"

#include <cstdio>
#include <iostream>
#include <algorithm>

#include "stb_image.h"

#define HZGL_LOG_ERROR(msg) \
    fprintf(stderr, "[ERROR] %s (line %d): %s\n", __FILE__, __LINE__, msg);

hzgl::ResourceManager::ResourceManager()
{
}

hzgl::ResourceManager::~ResourceManager()
{
    ReleaseAll();
}

void hzgl::ResourceManager::ReleaseAll()
{
    // delete all used VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(static_cast<GLsizei>(_usedVBOs.size()), _usedVBOs.data());

    // delete all used VAOs
    glBindVertexArray(0);
    glDeleteVertexArrays(static_cast<GLsizei>(_usedVAOs.size()), _usedVAOs.data());

    // delete all shader programs
    glUseProgram(0);
    for (const auto &pair : _programInfo)
        glDeleteProgram(pair.second.id);

    // delete all stages
    for (const auto &pair : _shaderInfo)
        glDeleteShader(pair.second.id);

    // delete all loaded textures
    glBindTexture(GL_TEXTURE_2D, 0);
    for (const auto &pair : _textureInfo)
        glDeleteTextures(1, &pair.second.id);
}

GLuint hzgl::ResourceManager::LoadShader(const std::string &filepath, GLenum shaderType)
{
    // avoid loading the same shader multiple times
    if (_shaderInfo.find(filepath) != _shaderInfo.end())
        return _shaderInfo[filepath].id;

    if (!Exists(filepath))
    {
        HZGL_LOG_ERROR("File does not exist.")
        return 0;
    }

    ShaderInfo shaderInfo;
    shaderInfo.filepath = filepath;
    shaderInfo.id = CreateShader(filepath, shaderType);

    _shaderInfo[filepath] = shaderInfo;

    return shaderInfo.id;
}

GLuint hzgl::ResourceManager::LoadShaderProgram(std::vector<ShaderStage> stages, const char *name)
{
    GLuint programID = glCreateProgram();

    for (auto &stage : stages)
    {
        GLuint shaderID = LoadShader(stage.filepath, stage.type);
        glAttachShader(programID, shaderID);
        stage.id = shaderID;
    }

    glLinkProgram(programID);

    GLint infoLogLength;
    GLint linkStatus = GL_FALSE;

    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0)
    {
        char *infoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programID, infoLogLength, NULL, infoLog);
        std::cout << infoLog << std::endl;
        delete[] infoLog;
    }

    std::string programName;

    if (name != nullptr)
        programName = name;
    else
        programName = "program " + std::to_string(_loadedPrograms.size());

    ProgramInfo progInfo;
    progInfo.id = programID;
    progInfo.stages = stages;
    progInfo.name = programName;

    _loadedPrograms.push_back(programName);
    _programInfo[programName] = progInfo;

    return progInfo.id;
}

GLuint hzgl::ResourceManager::LoadTexture(const std::string &filepath, GLenum type)
{
    // avoid loading the same texture multiple times
    if (_textureInfo.find(filepath) != _textureInfo.end())
        return _textureInfo[filepath].id;

    if (!Exists(filepath))
    {
        HZGL_LOG_ERROR("File does not exist.")
        return 0;
    }

    TextureInfo texInfo;
    TextureFromFile(filepath, type, &texInfo);

    _loadedTextures.push_back(filepath);
    _textureInfo[filepath] = texInfo;

    return texInfo.id;
}

void hzgl::ResourceManager::LoadMesh(const std::string &filepath, std::vector<RenderObject> &objects)
{
    enum Buffer_IDs
    {
        Position = 0,
        Normal,
        TexCoord,
        NumBuffers
    };

    enum Attrib_IDs
    {
        vPosition = 0,
        vNormal,
        vTexCoord,
        NumAttribs
    };

    if (_renderObjectInfo.find(filepath) != _renderObjectInfo.end())
        return;

    std::vector<MeshInfo> meshes;

    LoadOBJ(filepath, meshes);

    for (const auto &mesh : meshes)
    {
        RenderObject renderObj;
        renderObj.name = mesh.name;
        renderObj.num_vertices = mesh.num_vertices;
        renderObj.shading_mode = mesh.shading_mode;

        GLuint Buffers[NumBuffers] = {};

        // generate buffers
        glGenVertexArrays(1, &renderObj.VAO);
        glGenBuffers(NumBuffers, &Buffers[0]);

        glBindVertexArray(renderObj.VAO);

        // feed data to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Position]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.positions.size(), mesh.positions.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normal]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[TexCoord]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.texcoords.size(), mesh.texcoords.data(), GL_STATIC_DRAW);

        // VBO plumbing (assume the layout to be fixed)
        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Position]);
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
        glEnableVertexAttribArray(vPosition);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normal]);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
        glEnableVertexAttribArray(vNormal);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[TexCoord]);
        glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (void *)(0));
        glEnableVertexAttribArray(vTexCoord);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        for (const auto &pair : mesh.texpath)
        {
            const auto &type = pair.first;
            const auto &path = pair.second;

            // load each texture image and convert it to OpenGL handle
            if (!path.empty())
                renderObj.texture[type] = LoadTexture(path, GL_TEXTURE_2D);
            else
                renderObj.texture[type] = 0;
        }

        // keep track of the OpenGL handles used
        _usedVAOs.push_back(renderObj.VAO);

        for (int i = 0; i < NumBuffers; i++)
            _usedVBOs.push_back(Buffers[i]);

        objects.push_back(renderObj);
    }

    RenderObjectInfo renderObjInfo;
    renderObjInfo.filepath = filepath;
    renderObjInfo.start_index = static_cast<int>(_loadedMeshes.size());
    renderObjInfo.num_meshes = static_cast<int>(meshes.size());

    _loadedMeshes.push_back(filepath);
    _renderObjectInfo[filepath] = renderObjInfo;
}

std::vector<std::string> hzgl::ResourceManager::GetLoadedMeshesNames()
{
    return _loadedMeshes;
}

std::vector<std::string> hzgl::ResourceManager::GetLoadedTextureNames()
{
    return _loadedTextures;
}

std::vector<std::string> hzgl::ResourceManager::GetLoadedShaderProgramNames()
{
    return _loadedPrograms;
}

GLuint hzgl::ResourceManager::GetProgramID(int index)
{
    if (index < 0 || index >= _loadedPrograms.size())
    {
        HZGL_LOG_ERROR("Invalid index.")
        return 0;
    }

    std::string name = _loadedPrograms[index];

    return GetProgramID(name);
}

GLuint hzgl::ResourceManager::GetProgramID(const std::string& name)
{
    if (_programInfo.find(name) == _programInfo.end())
    {
        HZGL_LOG_ERROR("Invalid name.")
        return 0;
    }

    return _programInfo[name].id;
}

GLuint hzgl::ResourceManager::GetTextureID(int index)
{
    if (index < 0 || index >= _loadedTextures.size())
    {
        HZGL_LOG_ERROR("Invalid index.")
        return 0;
    }

    std::string name = _loadedTextures[index];

    return GetTextureID(name);
}

GLuint hzgl::ResourceManager::GetTextureID(const std::string& name)
{
    if (_textureInfo.find(name) == _textureInfo.end())
    {
        HZGL_LOG_ERROR("Invalid name.")
        return 0;
    }

    return _textureInfo[name].id;
}

int hzgl::ResourceManager::GetTextureWidth(int index){
    if (index < 0 || index >= _loadedTextures.size())
    {
        HZGL_LOG_ERROR("Invalid index.")
        return 0;
    }

    std::string name = _loadedTextures[index];

    return GetTextureWidth(name);
}
int hzgl::ResourceManager::GetTextureWidth(const std::string& name)
{
    if (_textureInfo.find(name) == _textureInfo.end())
    {
        HZGL_LOG_ERROR("Invalid name.")
        return 0;
    }

    return _textureInfo[name].width;
}

int hzgl::ResourceManager::GetTextureHeight(int index){
    if (index < 0 || index >= _loadedTextures.size())
    {
        HZGL_LOG_ERROR("Invalid index.")
        return 0;
    }

    std::string name = _loadedTextures[index];

    return GetTextureHeight(name);
}

int hzgl::ResourceManager::GetTextureHeight(const std::string& name)
{
    if (_textureInfo.find(name) == _textureInfo.end())
    {
        HZGL_LOG_ERROR("Invalid name.")
        return 0;
    }

    return _textureInfo[name].height;
}

#undef HZGL_LOG_ERROR
