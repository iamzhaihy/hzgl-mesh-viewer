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

    typedef struct 
    {
        GLuint id;
        int width;
        int height;
        int num_attachment;
        int num_color_attachment;
        int num_depth_attachment;
        int num_stencil_attachment;
        std::vector<GLuint> color_attachment;
        GLuint depth_attachment;
        GLuint stencil_attachment;
    } FrameBufferInfo;

    GLuint CreateFBO(int width, int height, FrameBufferInfo* fbInfo = nullptr);
    GLuint CreateFBO(int width, int height, const std::vector<AttachedImage>& attachments, FrameBufferInfo* fbInfo = nullptr);
} // namespace hzgl