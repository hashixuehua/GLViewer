#include "MSAAFramebuffer.h"
#include "ViewerSetting.h"

MSAARender::MSAAFramebuffer::MSAAFramebuffer(int width, int height)
    : framebufferMSAA(0), textureColorBufferMultiSampled(0), rbo(0), mWidth(width), mHeight(height)
{
    initializeOpenGLFunctions();

    // configure MSAA framebuffer
    // --------------------------
    glGenFramebuffers(1, &framebufferMSAA);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);
    // create a multisampled color attachment texture
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ViewerSetting::sampleSieOfMSAA, GL_RGB, mWidth, mHeight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, ViewerSetting::sampleSieOfMSAA, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MSAARender::MSAAFramebuffer::~MSAAFramebuffer()
{
    if (framebufferMSAA != 0)
    {
        glDeleteFramebuffers(1, &framebufferMSAA);
    }

    if (textureColorBufferMultiSampled != 0)
    {
        glDeleteTextures(1, &textureColorBufferMultiSampled);
    }
    if (rbo != 0)
    {
        glDeleteRenderbuffers(1, &rbo);
    }
}

void MSAARender::MSAAFramebuffer::EnableWriting()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);
}

void MSAARender::MSAAFramebuffer::BlitFrameBuffer(GLuint target )
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
}
