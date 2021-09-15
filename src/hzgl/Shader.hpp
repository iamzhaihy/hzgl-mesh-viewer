#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

namespace hzgl
{
    typedef struct
    {
        GLenum type;
        std::string filepath;
        GLuint id;
    } ShaderStage;

    GLuint CreateShader(const std::string& filepath, GLenum shaderType);
    GLuint CreateShaderProgram(std::vector<ShaderStage> stages, std::vector<const char*> feedbackVaryings={});

    void SetSampler(GLuint programID, const char* uName, GLuint texID, int unit);

    void SetIntegerv(GLuint programID, const char* uName, int count, int* data);
    void SetFloatv(GLuint programID, const char* uName, int count, float* data);
    void SetMatrixv(GLuint programID, const char* uName, int dimension, float* data);

    void SetInteger(GLuint programID, const char* uName, int count, int i1, int i2=0, int i3=0, int i4=0);
    void SetFloat(GLuint programID, const char* uName, int count, float f1, float f2=0.0f, float f3=0.0f, float f4=0.0f);
} // namespace hzgl