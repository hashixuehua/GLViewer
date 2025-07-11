#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <QPoint>
#include <QVector4D>
#include <memory>

namespace MSAARender
{
	class MSAAFramebuffer : protected QOpenGLFunctions_4_5_Core
	{
	public:
		MSAAFramebuffer(int width, int height);
		~MSAAFramebuffer();

		void EnableWriting();
		void BlitFrameBuffer(GLuint target);

	private:
		unsigned int framebufferMSAA;
		unsigned int textureColorBufferMultiSampled;
		unsigned int rbo;

		int mWidth;
		int mHeight;
	};
}