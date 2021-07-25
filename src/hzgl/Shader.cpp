#include "Shader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

#include <glad/glad.h>

static std::string readFile(const std::string& filepath)
{
    std::string fileContent;
    std::ifstream fileStream(filepath, std::ios::in);

    if (fileStream.is_open())
    {
        std::stringstream sstr;
        sstr << fileStream.rdbuf();
        fileContent = sstr.str();
        fileStream.close();
    }

    return fileContent;
}

GLuint hzgl::CreateShader(const std::string& filepath, GLenum shaderType)
{
    std::string shaderCode = readFile(filepath);

    if (shaderCode.empty())
        return 0;

    GLuint shaderID = glCreateShader(shaderType);

    const char* shaderCodePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderCodePointer, NULL);
    glCompileShader(shaderID);

    GLint infoLogLength;
    GLint compileStatus = GL_FALSE;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char* infoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, infoLog);
        std::cout << infoLog << std::endl;
        delete[] infoLog;
    }

    return shaderID;
}

GLuint hzgl::CreateShaderProgram(std::vector<ShaderStage> stages)
{
    GLuint programID = glCreateProgram();

    for (auto& stage : stages)
    {
        GLuint shaderID = CreateShader(stage.filepath, stage.type);
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
        char* infoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programID, infoLogLength, NULL, infoLog);
        std::cout << infoLog << std::endl;
        delete[] infoLog;
    }

    for (auto& stage : stages)
    {
        glDetachShader(programID, stage.id);
        glDeleteShader(stage.id);
    }

    return programID;
}