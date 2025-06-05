#include "SelectionFramebuffer.h"
#include "ViewerSetting.h"

SelectionRender::SelectionFramebuffer::SelectionFramebuffer(int width, int height)
    : mWidth(width)
    , mHeight(height)
{
    initializeOpenGLFunctions();

    // Generate framebuffer
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    // Generate texture
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, mWidth, mHeight, 0, GL_RGBA_INTEGER, GL_INT, NULL);

    // Bind texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

    // Generate the texture object for the depth buffer
    glGenTextures(1, &mFrameDepthStencilTexture);
    glBindTexture(GL_TEXTURE_2D, mFrameDepthStencilTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mFrameDepthStencilTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "SelectionFramebuffer::SelectionFramebuffer: Could not create framebuffer!";
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SelectionRender::SelectionFramebuffer::~SelectionFramebuffer()
{
    if (mFramebuffer != 0)
    {
        glDeleteFramebuffers(1, &mFramebuffer);
    }

    if (mTexture != 0)
    {
        glDeleteTextures(1, &mTexture);
    }
    if (mFrameDepthStencilTexture != 0)
    {
        glDeleteTextures(1, &mFrameDepthStencilTexture);
    }
}

void SelectionRender::SelectionFramebuffer::EnableWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
}

void SelectionRender::SelectionFramebuffer::DisableWriting()
{
    // Bind back the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void SelectionRender::SelectionFramebuffer::Clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SelectionRender::SelectionFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

SelectionRender::CurveQueryInfo SelectionRender::SelectionFramebuffer::Query(const QPoint& queryPoint)
{
    CurveQueryInfo info;
    Bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(queryPoint.x() /** ViewerSetting::devicePixelRatio*/, (mHeight - queryPoint.y()) /** ViewerSetting::devicePixelRatio*/, 1, 1, GL_RGBA_INTEGER, GL_INT, &info);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return info;
}
