#include "Framebuffer.hpp"

#include <vector>
#include <string>
#include <iostream>

GLuint hzgl::CreateFBO(int width, int height, const std::vector<AttachedImage>& attachments, FrameBufferInfo* fbInfo)
{
    if (fbInfo != nullptr) 
    {
        fbInfo->id = 0;
        fbInfo->width = width;
        fbInfo->height = height;
        fbInfo->num_attachment = 0;
        fbInfo->num_color_attachment = 0;
        fbInfo->num_depth_attachment = 0;
        fbInfo->num_stencil_attachment = 0;
        fbInfo->color_attachment.clear();
        fbInfo->depth_attachment = 0;
        fbInfo->stencil_attachment = 0;
    }

    // create a framebuffer object
    GLuint fboID;
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    if (fbInfo != nullptr)
        fbInfo->id = fboID;

    for (const auto& info : attachments)
    {
        if (fbInfo != nullptr)
            fbInfo->num_attachment += 1;
    
        if (info.type == GL_TEXTURE_2D)
        {
            GLuint texID;
            glGenTextures(1, &texID);

            glBindTexture(GL_TEXTURE_2D, texID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, info.internal_format, width, height, 0, info.internal_format, GL_UNSIGNED_BYTE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, info.attachment_point, GL_TEXTURE_2D, texID, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (fbInfo != nullptr) 
            {
                fbInfo->num_color_attachment += 1;
                fbInfo->color_attachment.push_back(texID);
            }
        }

        else if (info.type == GL_RENDERBUFFER)
        {
            GLuint rboID;
            glGenRenderbuffers(1, &rboID);

            glBindRenderbuffer(GL_RENDERBUFFER, rboID);
            glRenderbufferStorage(GL_RENDERBUFFER, info.internal_format, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, info.attachment_point, GL_RENDERBUFFER, rboID);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            if (fbInfo != nullptr) {
                if (info.attachment_point == GL_DEPTH_STENCIL_ATTACHMENT) 
                {
                    fbInfo->num_depth_attachment = fbInfo->num_stencil_attachment = 1;
                    fbInfo->depth_attachment = fbInfo->stencil_attachment = rboID;
                }
                else if (info.attachment_point == GL_DEPTH_ATTACHMENT) 
                {
                    fbInfo->num_depth_attachment += 1;
                    fbInfo->depth_attachment = rboID;
                }
                else if (info.attachment_point == GL_STENCIL_ATTACHMENT) 
                {
                    fbInfo->num_stencil_attachment += 1;
                    fbInfo->stencil_attachment = rboID;
                }
            }
        }
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FBO is not complete." << std::endl;
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fboID;
}

GLuint hzgl::CreateFBO(int width, int height, FrameBufferInfo* fbInfo)
{
    std::vector<AttachedImage> attachments = {
        {GL_TEXTURE_2D,   GL_RGBA, GL_COLOR_ATTACHMENT0},
        {GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT}
    };

    return CreateFBO(width, height, attachments, fbInfo);
}