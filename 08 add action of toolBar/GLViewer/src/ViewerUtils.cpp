#include "ViewerUtils.h"
#include <QPainter>

void ViewerUtils::screenToWord(int px, int py, double depth, const QRect& viewport_rect, const QMatrix4x4& modelInvert, const QMatrix4x4& viewInvert, const QMatrix4x4& projectionInvert, QVector3D& wcsPt)
{
    float pt_x = ((float)px / viewport_rect.width()) * 2.f - 1.f;
    float pt_y = -((float)py / viewport_rect.height()) * 2.f + 1.f;

    QVector4D ray_clip(pt_x, pt_y, depth/*0.0*//*1.0*/, 1.0);
    QVector4D ray_eye = projectionInvert * ray_clip;

    //ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1.0, 0.0);

    ray_eye = viewInvert * ray_eye;//.toVector3D();

    ray_eye = modelInvert * ray_eye;

    if (ray_eye.w() != 0.0)
    {
        ray_eye.setX(ray_eye.x() / ray_eye.w());
        ray_eye.setY(ray_eye.y() / ray_eye.w());
        ray_eye.setZ(ray_eye.z() / ray_eye.w());
    }

    wcsPt = ray_eye.toVector3D();
}

void ViewerUtils::worldToScreen(const QVector3D& wcsPt, const QRect& viewport_rect, bool considerModelTrans, const QMatrix4x4& modelMat, const QMatrix4x4& viewMat, const QMatrix4x4& projectionMat, float& px, float& py, double& depth)
{
    QVector4D ray_eye(wcsPt, 1.0);
    if (considerModelTrans)
        ray_eye = modelMat * ray_eye;
    ray_eye = projectionMat * viewMat * ray_eye;

    depth = ray_eye.z() / ray_eye.w();

    px = (ray_eye.x() / ray_eye.w() + 1.f) / 2.f * viewport_rect.width();
    py = (ray_eye.y() / ray_eye.w() - 1.0) / -2.f * viewport_rect.height();
}

Vector3f ViewerUtils::normalizeToAxis(const Vector3f& dir, const Vector3f& localX, const Vector3f& localY, const Vector3f& localZ)
{
    double angleX = localX.Angle(dir);
    double angleY = localY.Angle(dir);
    double angleZ = localZ.Angle(dir);
    double rAng90 = PI / 2.0;
    bool bTX = angleX > rAng90;
    bool bTY = angleY > rAng90;
    bool bTZ = angleZ > rAng90;
    if (bTX)
        angleX = PI - angleX;
    if (bTY)
        angleY = PI - angleY;
    if (bTZ)
        angleZ = PI - angleZ;

    if (angleX < angleY && angleX < angleZ)
        return bTX ? -localX : localX;
    else if (angleY < angleX && angleY < angleZ)
        return bTY ? -localY : localY;
    else// if (angleZ < angleX && angleZ < angleY)
        return bTZ ? -localZ : localZ;
}

void ViewerUtils::getPickRay(int px, int py, const QRect& viewport_rect, QVector3D& cameraPos, const QMatrix4x4& modelInvert, const QMatrix4x4& viewInvert, const QMatrix4x4& projectionInvert, QVector3D& rayOri, QVector3D& rayEnd)
{
    float pt_x = ((float)px / viewport_rect.width()) * 2.f - 1.f;
    float pt_y = -((float)py / viewport_rect.height()) * 2.f + 1.f;

    QVector4D ray_clip(pt_x, pt_y, 1.0, 1.0);
    QVector4D ray_eye = projectionInvert * ray_clip;

    //ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1.0, 0.0);

    ray_eye = viewInvert * ray_eye;//.toVector3D();

    ray_eye = modelInvert * ray_eye;

    if (ray_eye.w() != 0.0)
    {
        ray_eye.setX(ray_eye.x() / ray_eye.w());
        ray_eye.setY(ray_eye.y() / ray_eye.w());
        ray_eye.setZ(ray_eye.z() / ray_eye.w());
    }

    QVector3D rayDirPt = ray_eye.toVector3D();

    rayOri = modelInvert * cameraPos;

    rayEnd = rayDirPt;
    //rayDir = rayDirPt - rayOri;
    //rayDir.normalize();

    //vec3 ray_dir = normalise(ray_world.xyz - camera_position);
}

QOpenGLTexture* ViewerUtils::getOrCreateImageTexture(const QString& imageFile, bool reverse/* = false*/)
{
    auto itrFind = mapImagePath2Texture.find(imageFile);
    if (itrFind != mapImagePath2Texture.end())
        return itrFind->second.texture;

    QImage image(imageFile);
    if (reverse)
        image.mirror();

    if (image.isNull())
        throw exception("error.");
    auto texture = new QOpenGLTexture(image/*.mirrored()*/);
    //auto texture = std::make_unique<QOpenGLTexture>(image/*.mirrored()*/);

    texture->create();
    //纹理环绕方式
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
    //纹理过滤
    texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
    //多级渐远纹理
    texture->generateMipMaps();
    texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);

    mapImagePath2Texture[imageFile] = ImageTexture(texture, image.width(), image.height());

    return texture;
}

QOpenGLTexture* ViewerUtils::getOrCreateTextTexture(const QString& text)
{
    auto itrFind = mapLabel2Texture.find(text);
    if (itrFind != mapLabel2Texture.end())
        return itrFind->second.texture;

    QImage image(256, 256, QImage::Format_RGBA8888);
    image.fill(Qt::transparent); // 透明背景

    // 在 QImage 上绘制文字
    QPainter painter;
    if (!painter.begin(&image)) return 0;

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 100));
    painter.drawText(image.rect(), Qt::AlignCenter, text);
    painter.end();

    bool valid = image.isNull();
    //auto texture = std::make_unique<QOpenGLTexture>(image/*.mirrored()*/);
    auto texture = new QOpenGLTexture(image/*.mirrored()*/);
    texture->create();
    //纹理环绕方式
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
    //纹理过滤
    texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
    //多级渐远纹理
    texture->generateMipMaps();
    texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);

    mapLabel2Texture[text] = ImageTexture(texture, image.width(), image.height());

    return texture;
}

map<QString, ImageTexture> ViewerUtils::mapLabel2Texture;
map<QString, ImageTexture> ViewerUtils::mapImagePath2Texture;

////////////

string ViewerCache::defaultWorkPlane = "defaultWorkPlane";
string ViewerCache::currentWorkPlane = "currentWorkPlane";
string ViewerCache::viewCube = "viewCube";
string ViewerCache::mouseLabel = "mouseLabel";



