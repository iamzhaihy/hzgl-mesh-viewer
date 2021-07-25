#pragma once

#include <string>

#include <glad/glad.h>

namespace hzgl
{
    typedef struct {
        GLuint id;
        int width;
        int height;
        int num_channels;
        std::string filepath;
    } TextureInfo;

    GLuint TextureFromFile(const std::string& filepath, GLenum type = GL_TEXTURE_2D, TextureInfo* texInfo = nullptr);
} // namespace hzgl
