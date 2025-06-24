#pragma once
#include <CGLib/TriangleMesh.h>

#include "CGLibUtils.h"
#include "ViewerUtils.h"
#include "ViewerSetting.h"

using namespace std;
using namespace CGUTILS;

class DrawingImageHolder
{
public:
	DrawingImageHolder(QOpenGLFunctions_4_5_Core* glFunc);
	~DrawingImageHolder();

	bool TryToSetup(const list<Line>& lstLine);
	void DrawImage(QOpenGLShaderProgram& shader);

private:
	void getImageSize(QString& file, int& width, int& height);
	void updateImageInfo(const QString& imageFile, double imageLength, double imageWidth, const Vector3f& localX, const Vector3f& localY, const Vector3f& normal, const Vector3f& insert, double scale);

public:
	//  image
	QString currentImageFile;

private:
	QOpenGLFunctions_4_5_Core* m_glFunc;

	map<QString, ImageTexture> mapImagePath2Texture;
	list<ImageInfo> images;
};
