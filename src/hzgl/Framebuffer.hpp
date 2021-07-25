#pragma once

#include <vector>

#include <glad/glad.h>

namespace hzgl
{
    typedef struct
    {
        GLenum type;             // e.g. Texture, Renderbuffer
        GLenum internal_format;  // e.g. RGBA8, Depth Component
        GLenum attachment_point; // e.g. GL_COLOR_ATTACHMENT0
        GLuint id;
    } AttachedImage;

    GLuint CreateFBO(int width, int height);
    GLuint CreateFBO(int width, int height, const std::vector<AttachedImage>& attachments);
} // namespace hzgl