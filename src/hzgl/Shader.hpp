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
    GLuint CreateShaderProgram(std::vector<ShaderStage> stages);
} // namespace hzgl