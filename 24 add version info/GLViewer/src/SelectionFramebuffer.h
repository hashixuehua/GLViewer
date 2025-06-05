#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QPoint>
#include <QVector4D>
#include <memory>

namespace SelectionRender
{
    struct CurveQueryInfo
    {
        int index;
        int unused0;
        int unused1;
        int result; // 0: Fail, 1: Success
    };

    class SelectionFramebuffer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        SelectionFramebuffer(int width, int height);
        ~SelectionFramebuffer();

        void EnableWriting();
        void DisableWriting();
        
        CurveQueryInfo Query(const QPoint& queryPoint);
        void Clear();

    private:
        void Bind();
        GLuint GetHandle() const { return mFramebuffer; }
        GLuint GetTexture() const { return mTexture; }

      private:
        GLuint mFramebuffer{ 0 };
        GLuint mFrameDepthStencilTexture{ 0 };
        GLuint mTexture{ 0 };

        int mWidth;
        int mHeight;
    };

    using CurveSelectionFramebufferPtr = std::shared_ptr<SelectionFramebuffer>;
}