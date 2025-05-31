#include "model.h"

Model::Model(QOpenGLFunctions_4_5_Core* glFunc):
    m_glFunc(glFunc)
{
    drawingLinesholder = new DrawingLinesHolder(glFunc);
    drawingViewerElemHolder = new DrawingViewerElemHolder(glFunc);
    drawingViewerElemHolder->setup();
    drawingImageHolder = new DrawingImageHolder(glFunc);
    drawingComponentHolder = new DrawingComponentHolder(glFunc);
    drawingComponentHolder->setup();
}

Model::~Model()
{
    delete drawingLinesholder;
    delete drawingViewerElemHolder;
    delete drawingImageHolder;
    delete drawingComponentHolder;
}

void Model::Draw2(QOpenGLShaderProgram& shader)
{
    m_glFunc->glLineWidth(2.0f);

    //  work plane
    //  由于工作平面有透明度，最后绘制
    drawingViewerElemHolder->drawWorkPlane(shader);

    drawingLinesholder->DrawLines(shader, ViewerSetting::previewData);
    drawingLinesholder->DrawSelectedLines(shader);

    drawingComponentHolder->draw(shader);
}

void Model::DrawSelectionInfo(QOpenGLShaderProgram& shader)
{
    drawingComponentHolder->drawSelectionInfo(shader);
}

void Model::DrawViewElement(QOpenGLShaderProgram& shader, const string& name)
{
    drawingViewerElemHolder->drawViewElement(shader, name);
}

void Model::DrawImage(QOpenGLShaderProgram& shader)
{
    drawingImageHolder->DrawImage(shader);
}

//////////////////////////////////////

////////// draw lines

void Model::BeginToDrawLines(GLCurveType curveType, int drawType/* = -1*/)
{
    drawingLinesholder->BeginToDrawLines(curveType, drawType);

    ViewerSetting::previewData.curveType = curveType;
    ViewerSetting::previewData.drawType = drawType;
    ViewerSetting::previewData.wkPlaneNormal = ViewerSetting::currentWorkPlNormal;
}

void Model::AddVertexToLines(const CommandPara& para)
{
    ViewerSetting::previewData.addPara(para);
    if (ViewerSetting::previewData.complete)
    {
        //lines.at(norDefualt).back().vertex.push_back(vert);
        addCompletePreviewDataToLines();
        ViewerSetting::previewData.update(para);
    }

    //if (previewData.curveType == GLCurveType::GLTemp && ViewerSetting::viewerStatus == ViewerStatus::Copy && !basePt4Copy)
    //{
    //    basePt4Copy = new Vector3f(previewData.curData.back().pValue);
    //}
}

void Model::SetPreviewNextPoint(double x, double y, double z)
{
    if (ViewerSetting::previewData.previewNextPt == nullptr)
        ViewerSetting::previewData.previewNextPt = new Vector3f();

    ViewerSetting::previewData.previewNextPt->X = x;
    ViewerSetting::previewData.previewNextPt->Y = y;
    ViewerSetting::previewData.previewNextPt->Z = z;

    if (!ViewerSetting::snapMode)
        return;

    //  捕捉吸附
    drawingLinesholder->GetSnapPoint(*ViewerSetting::previewData.previewNextPt);
}

void Model::EndOfDrawingLine()
{
    drawingLinesholder->EndOfDrawingLine();
}

bool Model::IsLinePreviewValid()
{
    //if (lines.at(norDefualt).empty() || lines.at(norDefualt).back().vertex.size() < 1)
    //    return false;
    //if (previewData.curveType == GLCurveType::GLUnknown || previewData.curData.size() < 1)
    //    return false;
    if (ViewerSetting::previewData.curveType == GLCurveType::GLUnknown /*|| previewData.curData.size() < 1*/)
        return false;

    return true;
}

void Model::addCompletePreviewDataToLines()
{
    //  必须是complete，这里不做校验，由调用者保证

    list<Line> lstLine;
    ViewerSetting::previewData.getCompleteLines(lstLine);

    excuteAction(lstLine);

}

void Model::excuteAction(const list<Line>& lstLine)
{
    if (drawingImageHolder->TryToSetup(lstLine))
        return;

    //if (previewData.curveType == GLCurveType::GLPoint)
    //{
    //    if (ViewerSetting::viewerStatus == ViewerStatus::CSphere)
    //    {
    //        CreateSphere(1000.0 * lstLine.begin()->pt1 + bottomOffset * currentWorkPlNormal);
    //        return;
    //    }
    //}

    ////  copy等只需要预览的，在“画线”操作完成后执行copy
    //if (previewData.curveType == GLCurveType::GLTemp)
    //{
    //    if (ViewerSetting::viewerStatus == ViewerStatus::Copy)
    //    {
    //        if (!basePt4Copy)
    //            basePt4Copy = new Vector3f(lstLine.back().pt0);

    //        Vector3f offset = lstLine.back().pt1 - *basePt4Copy;
    //        CopySelectedData(offset);
    //        return;
    //    }
    //    else if (ViewerSetting::viewerStatus == ViewerStatus::DrawingImage)
    //    {
    //        Vector3f dirX = lstLine.back().pt1 - lstLine.back().pt0;
    //        double len = dirX.Length();

    //        int imageWid = 0, imageHei = 0;
    //        getImageSize(currentImageFile, imageWid, imageHei);

    //        double ratio = ((double)imageHei) / imageWid;
    //        double height = len * ratio;

    //        dirX.Normalize();
    //        Vector3f dirY = currentWorkPlNormal.CrossProduct(dirX);
    //        dirY.Normalize();
    //        Vector3f insert = 0.5 * (lstLine.back().pt1 + lstLine.back().pt0) + dirY * (0.5 * height);

    //        double scale = 1000.0 * len / imageWid;
    //        UpdateImageInfo(currentImageFile, imageWid, imageHei, dirX, dirY, currentWorkPlNormal, 1000.0 * insert, scale);
    //        return;
    //    }
    //}

    drawingLinesholder->pushCurveData(ViewerSetting::previewData.curveType, lstLine);
}

void Model::GetOrthogonalityPoint(Vector3f& cur)
{
    if (!ViewerSetting::orthogonalityMode || ViewerSetting::previewData.curData.empty())
        return;

    //  TODO 需要把局部坐标系存到工作平面中，统一规范使用
    Vector3f localX, localY;
    CGUtils::getLocalXY(ViewerSetting::currentWorkPlNormal, localX, localY);

    auto& pre = ViewerSetting::previewData.curData.back().pValue;
    Vector3f dir = cur - pre;

    Vector3f paraAxis = ViewerUtils::normalizeToAxis(dir, localX, localY, ViewerSetting::currentWorkPlNormal);
    cur = pre + dir.DotProduct(paraAxis) * paraAxis;
}

//////////////////////////////////////

void Model::SetImageDrawingFile(const QString& image)
{
    drawingImageHolder->currentImageFile = image;
}

const BodyInfo* Model::GetViewCubeBody()
{
    return drawingViewerElemHolder->GetViewCubeBody();
}

void Model::SetSelected(int elementId, bool addMode)
{
    drawingComponentHolder->setSelected(elementId, addMode);
}

void Model::SetWorkPlane(const Vector3f& rayOri, const Vector3f& rayEnd)
{
    list<const BodyInfo*> lstBody;
    drawingComponentHolder->getSelectedBody(lstBody);

    Vector3f planePt, planeNormal;
    float offset;
    if (lstBody.size() != 1 || !CGLibUtils::getClickHittedPlane(rayOri, rayEnd, *lstBody.back(), planePt, planeNormal))
    {
        planePt = Vector3f::Zero;
        planeNormal = Vector3f::BasicZ;
        offset = 0.0f;
    }
    else
    {
        offset = -1.8f;
    }

    updateCurrentWorkPlaneData(planePt, planeNormal);
    drawingViewerElemHolder->setCurrentWorkPlane(planePt, planeNormal, offset);
}

void Model::updateCurrentWorkPlaneData(const Vector3f& planePt, const Vector3f& planeNormal)
{
    ViewerSetting::currentWorkPlPoint = planePt;
    ViewerSetting::currentWorkPlNormal = planeNormal;
    drawingLinesholder->setCurrentWorkPlaneNormal(ViewerSetting::currentWorkPlNormal);
}

void Model::ClearSelected()
{
    drawingComponentHolder->clearSelected();
}

