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

GLuint hzgl::ResourceManager::LoadTexture(const std::string &filepath, GLenum type)
{
    // avoid loading the same texture multiple times
    if (_textureInfo.find(filepath) != _textureInfo.end())
        return _textureInfo[filepath].id;

    std::cout << "Loading texture from " << filepath << std::endl;

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

GLuint hzgl::ResourceManager::LoadShader(const std::string &filepath, GLenum shaderType)
{
    // avoid loading the same shader multiple times
    if (_shaderInfo.find(filepath) != _shaderInfo.end())
        return _shaderInfo[filepath].id;

    std::cout << "Loading shader from " << filepath << std::endl;

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

void hzgl::ResourceManager::LoadModel(const std::string &filepath, std::vector<RenderObject> &objects, const char* name, bool duplicateAllowed)
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

    if (!duplicateAllowed && _renderObjects.find(filepath) != _renderObjects.end())
        return;

    std::cout << "Loading meshes from " << filepath << std::endl;

    std::vector<MeshInfo> shapes;

    LoadMeshesFromFile(filepath, shapes);

    RenderObject renderObject;
    for (const auto &shape : shapes)
    {
        RenderShape renderShape;
        renderShape.name = shape.name;
        renderShape.num_indices = shape.indices.size();
        renderShape.num_vertices = shape.num_vertices;
        renderShape.shading_mode = shape.shading_mode;
        renderShape.has_normals = shape.normals.size() > 0;
        renderShape.has_texcoords = shape.texcoords.size() > 0;

        GLuint Buffers[NumBuffers] = {};

        // generate buffers
        glGenVertexArrays(1, &renderShape.VAO);
        glGenBuffers(NumBuffers, &Buffers[0]);

        glBindVertexArray(renderShape.VAO);

        // feed data to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Position]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape.positions.size(), shape.positions.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normal]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape.normals.size(), shape.normals.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[TexCoord]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape.texcoords.size(), shape.texcoords.data(), GL_STATIC_DRAW);

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

        glGenBuffers(1, &renderShape.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderShape.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shape.indices.size(), shape.indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        for (const auto &pair : shape.texpath)
        {
            const auto &type = pair.first;
            const auto &path = pair.second;

            // load each texture image and convert it to OpenGL handle
            if (!path.empty())
                renderShape.texture[type] = LoadTexture(path, GL_TEXTURE_2D);
            else
                renderShape.texture[type] = 0;

            if (renderShape.texture[type] > 0)
                renderShape.has_textures = true;
        }

        // keep track of the OpenGL handles used
        _usedVAOs.push_back(renderShape.VAO);

        for (int i = 0; i < NumBuffers; i++)
            _usedVBOs.push_back(Buffers[i]);

        renderObject.shapes.push_back(renderShape);
    }

    renderObject.path = filepath;
    renderObject.num_shapes = renderObject.shapes.size();

    objects.push_back(renderObject);

    _loadedMeshes.push_back(filepath);
    _renderObjects[(name ? std::string(name) : filepath)] = renderObject;
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

const hzgl::ProgramInfo& hzgl::ResourceManager::GetProgramInfo(int index)
{
    if (index < 0 || index >= _loadedPrograms.size())
    {
        HZGL_LOG_ERROR("Invalid index.")
        return GetProgramInfo(0);
    }

    std::string name = _loadedPrograms[index];

    return GetProgramInfo(name);
}

const hzgl::ProgramInfo& hzgl::ResourceManager::GetProgramInfo(const std::string& name)
{
    if (_programInfo.find(name) == _programInfo.end())
    {
        HZGL_LOG_ERROR("Invalid name.")
        return GetProgramInfo(0);
    }

    return _programInfo[name];
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
