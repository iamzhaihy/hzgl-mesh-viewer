#include "Framebuffer.hpp"

#include <vector>
#include <string>
#include <iostream>

GLuint hzgl::CreateFBO(int width, int height, const std::vector<AttachedImage>& attachments)
{
    // create a framebuffer object
    GLuint fboID;
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    for (const auto& info : attachments)
    {
        if (info.type == GL_TEXTURE_2D)
        {
            GLuint texID;
            glGenTextures(1, &texID);

            glBindTexture(info.type, texID);
            glTexParameteri(info.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(info.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(info.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(info.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(info.type, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexImage2D(info.type, 0, info.internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glBindTexture(info.type, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, info.attachment_point, info.type, texID, 0);
        }

        else if (info.type == GL_RENDERBUFFER)
        {
            GLuint rboID;
            glGenRenderbuffers(1, &rboID);

            glBindRenderbuffer(GL_RENDERBUFFER, rboID);
            glRenderbufferStorage(GL_RENDERBUFFER, info.type, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, info.attachment_point, info.type, rboID);
        }
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FBO is not complete.\n";
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fboID;
}

GLuint hzgl::CreateFBO(int width, int height)
{
    std::vector<AttachedImage> attachments = { {GL_TEXTURE_2D, GL_RGBA8, GL_COLOR_ATTACHMENT0},
                                                  {GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT} };

    return CreateFBO(width, height, attachments);
}