#include "Texture.hpp"

#include <string>
#include <iostream>
#include <algorithm>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static GLenum hzglImageFormat(const std::string &filepath, int numChannels)
{
    if (numChannels < 0 || numChannels > 4)
    {
        std::cerr << "Invalid argument: " << numChannels << std::endl;
        return 0;
    }

    std::size_t lastDot = filepath.find_last_of('.');
    std::string ext = filepath.substr(lastDot + 1);

    // convert the extension to lowercase for easier comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });

    // TODO: use extention to make an educated guess about the internal format

    if (numChannels == 1)
    {
        return GL_RED;
    }
    else if (numChannels == 2)
    {
        // TODO: GR
        return GL_RG;
    }
    else if (numChannels == 3)
    {
        // TODO: BGR
        return GL_RGB;
    }
    else
    {
        // TODO: BGRA
        return GL_RGBA;
    }
}

GLuint hzgl::TextureFromFile(const std::string &filepath, GLenum type, TextureInfo* texInfo)
{
    int width, height, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &n, 0);

    if (!data)
    {
        std::cerr << "Failed to load " << filepath << std::endl;
        return 0;
    }

    GLuint format, internalFormat;

    if (n == 1)
        format = GL_RED;
    else if (n == 2)
        format = GL_RG;
    else if (n == 3)
        format = GL_RGB;
    else
        format = GL_RGBA;

    internalFormat = hzglImageFormat(filepath, n);

    GLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(type, texID);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(type, 0, format, width, height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(type);
    glBindTexture(type, 0);

    if (texInfo != nullptr)
    {
        texInfo->filepath = filepath;
        texInfo->id = texID;
        texInfo->width = width;
        texInfo->height = height;
        texInfo->num_channels = n;
    }

    stbi_image_free(data);

    return texID;
}