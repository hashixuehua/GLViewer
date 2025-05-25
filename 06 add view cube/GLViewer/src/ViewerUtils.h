#pragma once
#include <string>
#include <QOpenGLTexture>
#include "CGUtils/CGUtils.h"
#include "CGLibUtils.h"

using namespace std;
using namespace CGUTILS;

class ViewerUtils
{
public:
    static Vector3f normalizeToAxis(const Vector3f& dir, const Vector3f& localX, const Vector3f& localY, const Vector3f& localZ);
    static void getPickRay(int px, int py, const QRect& viewport_rect, QVector3D& cameraPos, const QMatrix4x4& modelInvert, const QMatrix4x4& viewInvert, const QMatrix4x4& projectionInvert, QVector3D& rayOri, QVector3D& rayEnd);

    static QOpenGLTexture* getOrCreateImageTexture(const QString& imageFile, bool reverse = false);
    static QOpenGLTexture* getOrCreateTextTexture(const QString& text);


private:
    static map<QString, ImageTexture> mapLabel2Texture;
    static map<QString, ImageTexture> mapImagePath2Texture;

};

class ViewerCache
{
public:
    static string defaultWorkPlane;
    static string currentWorkPlane;
    static string viewCube;


};
