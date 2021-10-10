#include "Shader.hpp"

#include <cassert>
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

GLuint hzgl::CreateShaderProgram(std::vector<ShaderStage> stages, std::vector<const char*> feedbackVaryings)
{
    GLuint programID = glCreateProgram();

    for (auto& stage : stages)
    {
        GLuint shaderID = CreateShader(stage.filepath, stage.type);
        glAttachShader(programID, shaderID);
        stage.id = shaderID;
    }

    if (!feedbackVaryings.empty())
        glTransformFeedbackVaryings(programID, feedbackVaryings.size(), feedbackVaryings.data(), GL_INTERLEAVED_ATTRIBS);

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

void hzgl::SetSampler(GLuint programID, const char* uName, GLuint texID, int unit) 
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && unit >= 0);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(loc, unit);
}

void hzgl::SetIntegerv(GLuint programID, const char* uName, int count, int* data) 
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && 1 <= count && count <= 4);

    switch (count)
    {
        case 1:
            glUniform1iv(loc, 1, data);
            break;
        case 2:
            glUniform2iv(loc, 1, data);
            break;
        case 3:
            glUniform3iv(loc, 1, data);
            break;
        case 4:
            glUniform4iv(loc, 1, data);
            break;
        default:
            break;
    }
}

void hzgl::SetFloatv(GLuint programID, const char* uName, int count, float* data) 
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && 1 <= count && count <= 4);

    switch (count)
    {
        case 1:
            glUniform1fv(loc, 1, data);
            break;
        case 2:
            glUniform2fv(loc, 1, data);
            break;
        case 3:
            glUniform3fv(loc, 1, data);
            break;
        case 4:
            glUniform4fv(loc, 1, data);
            break;
        default:
            break;
    }
}

void hzgl::SetMatrixv(GLuint programID, const char* uName, int dimension, float* data) 
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && 2 <= dimension && dimension <= 4);

    switch (dimension)
    {
        case 2:
            glUniformMatrix2fv(loc, 1, GL_FALSE, data);
            break;
        case 3:
            glUniformMatrix3fv(loc, 1, GL_FALSE, data);
            break;
        case 4:
            glUniformMatrix4fv(loc, 1, GL_FALSE, data);
            break;
        default:
            break;
    }
}

void hzgl::SetInteger(GLuint programID, const char* uName, int count, int i1, int i2, int i3, int i4)
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && 1 <= count && count <= 4);

    switch (count)
    {
        case 1:
            glUniform1i(loc, i1);
            break;
        case 2:
            glUniform2i(loc, i1, i2);
            break;
        case 3:
            glUniform3i(loc, i1, i2, i3);
            break;
        case 4:
            glUniform4i(loc, i1, i2, i3, i4);
            break;
        default:
            break;
    }
}

void hzgl::SetFloat(GLuint programID, const char* uName, int count, float f1, float f2, float f3, float f4)
{
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    GLint loc = glGetUniformLocation(programID, uName);
    assert(loc != -1 && 1 <= count && count <= 4);

    switch (count)
    {
        case 1:
            glUniform1f(loc, f1);
            break;
        case 2:
            glUniform2f(loc, f1, f2);
            break;
        case 3:
            glUniform3f(loc, f1, f2, f3);
            break;
        case 4:
            glUniform4f(loc, f1, f2, f3, f4);
            break;
        default:
            break;
    }
}
