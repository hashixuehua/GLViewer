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
    static void screenToWord(int px, int py, double depth, const QRect& viewport_rect, const QMatrix4x4& modelInvert, const QMatrix4x4& viewInvert, const QMatrix4x4& projectionInvert, QVector3D& wcsPt);
    static void worldToScreen(const QVector3D& wcsPt, const QRect& viewport_rect, bool considerModelTrans, const QMatrix4x4& modelMat, const QMatrix4x4& viewMat, const QMatrix4x4& projectionMat, float& px, float& py, double& depth);

    static Vector3f normalizeToAxis(const Vector3f& dir, const Vector3f& localX, const Vector3f& localY, const Vector3f& localZ);
    static void getPickRay(int px, int py, const QRect& viewport_rect, QVector3D& cameraPos, const QMatrix4x4& modelInvert, const QMatrix4x4& viewInvert, const QMatrix4x4& projectionInvert, QVector3D& rayOri, QVector3D& rayEnd);
    static bool getIntersection(const QVector3D& rayOri, const QVector3D& rayEnd, const Plane& plane, QVector3D& inter);

    static bool isNumber(const std::string& str);
    static void Stringsplit(const std::string& str, const std::string& split, vector<std::string>& res);
    static void Stringsplit(const std::string& str, const char split, vector<std::string>& res);

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
    static string mouseLabel;
    static string snapLabel;

};
