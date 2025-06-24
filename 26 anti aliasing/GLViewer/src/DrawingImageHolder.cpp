#include "DrawingImageHolder.h"

DrawingImageHolder::DrawingImageHolder(QOpenGLFunctions_4_5_Core* glFunc) :m_glFunc(glFunc)
{
}

DrawingImageHolder::~DrawingImageHolder() {}

void DrawingImageHolder::DrawImage(QOpenGLShaderProgram& shader)
{
    for (auto& image : images)
    {
        // 纹理
        auto textTexture = ViewerUtils::getOrCreateImageTexture(image.imageFile);
        textTexture->bind(0);

        shader.setUniformValue("objectColor", image.color);  //所示面颜色的反色，这样突出文字显示
        shader.setUniformValue("texture_diffuse1", 0);

        image.mesh->Draw(shader);

        textTexture->release();
    }
}

bool DrawingImageHolder::TryToSetup(const list<Line>& lstLine)
{
    if (ViewerSetting::previewData.curveType == GLCurveType::GLTemp && ViewerSetting::viewerStatus == ViewerStatus::DrawingImage)
    {
        Vector3f dirX = lstLine.back().pt1 - lstLine.back().pt0;
        double len = dirX.Length();

        int imageWid = 0, imageHei = 0;
        getImageSize(currentImageFile, imageWid, imageHei);

        double ratio = ((double)imageHei) / imageWid;
        double height = len * ratio;

        dirX.Normalize();
        Vector3f dirY = ViewerSetting::currentWorkPlNormal.CrossProduct(dirX);
        dirY.Normalize();
        Vector3f insert = 0.5 * (lstLine.back().pt1 + lstLine.back().pt0) + dirY * (0.5 * height);

        double scale = 1000.0 * len / imageWid;
        updateImageInfo(currentImageFile, imageWid, imageHei, dirX, dirY, ViewerSetting::currentWorkPlNormal, 1000.0 * insert, scale);
        return true;
    }

    return false;
}

void DrawingImageHolder::updateImageInfo(const QString& imageFile, double imageLength, double imageWidth, const Vector3f& localX, const Vector3f& localY, const Vector3f& normal, const Vector3f& insert, double scale)
{
    TriangleMesh mesh;
    CGLibUtils::GetImageMesh(Vector3f::BasicZ, 0.5 * imageLength, 0.5 * imageWidth, Vector3f::Zero, 0.0, 0.01f, mesh);

    //  绕图片中心旋转
    Transform trs(scale * localX, scale * localY, scale * normal, Vector3f::Zero);
    //Transform trsScale(scale * Vector3f::BasicX, scale * Vector3f::BasicY, scale * Vector3f::BasicZ, Vector3f::Zero);
    //Transform trs;
    //Transform::Mult(trsRotate, trsScale, trs);
    trs.SetTranslate(insert);

    //Transform trs(insert, normal, rotation);
    mesh.Transform(trs);

    auto cpMesh = CGLibUtils::ConvertMesh(m_glFunc, mesh);

    images.push_back(ImageInfo(imageFile, cpMesh, normal, QVector4D(1.f, 1.f, 1.f, 1.f), trs));
}

void DrawingImageHolder::getImageSize(QString& file, int& width, int& height)
{
    auto itrFind = mapImagePath2Texture.find(file);
    if (itrFind != mapImagePath2Texture.end())
    {
        width = itrFind->second.originWdith;
        height = itrFind->second.originHeight;
        return;
    }

    QImage image(currentImageFile);
    if (image.isNull())
        return;

    width = image.width();
    height = image.height();
}