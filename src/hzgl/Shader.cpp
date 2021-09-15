#include "Shader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

#include <glad/glad.h>

static bool hzglInRange(int x, int lo, int hi)
{
    return (x >= lo) && (x <= hi);
}

static bool hzglCheckUniform(GLuint programID, const char* uName)
{
    return glGetUniformLocation(programID, uName) != -1;
}

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

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(glGetUniformLocation(programID, uName), unit);
}

void hzgl::SetIntegerv(GLuint programID, const char* uName, int count, int* data) 
{
    if (count < 1)
    {
        std::cerr << "Error: invalid argument" << std::endl;
        return;
    }

    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    glUniform1iv(glGetUniformLocation(programID, uName), count, data);
}

void hzgl::SetFloatv(GLuint programID, const char* uName, int count, float* data) 
{
    if (count < 1)
    {
        std::cerr << "Error: invalid argument" << std::endl;
        return;
    }

    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    glUniform1fv(glGetUniformLocation(programID, uName), count, data);
}

void hzgl::SetMatrixv(GLuint programID, const char* uName, int dimension, float* data) 
{
    if (!hzglInRange(dimension, 2, 4))
    {
        std::cerr << "Error: invalid argument" << std::endl;
        return;
    }
   
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    switch (dimension)
    {
        case 2:
            glUniformMatrix2fv(glGetUniformLocation(programID, uName), 1, GL_FALSE, data);
            break;
        case 3:
            glUniformMatrix3fv(glGetUniformLocation(programID, uName), 1, GL_FALSE, data);
            break;
        case 4:
            glUniformMatrix4fv(glGetUniformLocation(programID, uName), 1, GL_FALSE, data);
            break;
        default:
            break;
    }
}

void hzgl::SetInteger(GLuint programID, const char* uName, int count, int i1, int i2, int i3, int i4)
{
    if (count < 1)
    {
        std::cerr << "Error: invalid argument" << std::endl;
        return;
    }
   
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    switch (count)
    {
        case 1:
            glUniform1i(glGetUniformLocation(programID, uName), i1);
            break;
        case 2:
            glUniform2i(glGetUniformLocation(programID, uName), i1, i2);
            break;
        case 3:
            glUniform3i(glGetUniformLocation(programID, uName), i1, i2, i3);
            break;
        case 4:
            glUniform4i(glGetUniformLocation(programID, uName), i1, i2, i3, i4);
            break;
        default:
            break;
    }
}

void hzgl::SetFloat(GLuint programID, const char* uName, int count, float f1, float f2, float f3, float f4)
{
    if (!hzglInRange(count, 1, 4))
    {
        std::cerr << "Error: invalid argument" << std::endl;
        return;
    }
   
    static GLuint prevProgramID = 0;

    if (prevProgramID != programID) 
    {
        glUseProgram(programID);
        prevProgramID = programID;
    }

    if (!hzglCheckUniform(programID, uName)) {
        std::cerr << "Error: uniform " << uName << " does not exist." << std::endl;
        return;
    }

    switch (count)
    {
        case 1:
            glUniform1f(glGetUniformLocation(programID, uName), f1);
            break;
        case 2:
            glUniform2f(glGetUniformLocation(programID, uName), f1, f2);
            break;
        case 3:
            glUniform3f(glGetUniformLocation(programID, uName), f1, f2, f3);
            break;
        case 4:
            glUniform4f(glGetUniformLocation(programID, uName), f1, f2, f3, f4);
            break;
        default:
            break;
    }
}
