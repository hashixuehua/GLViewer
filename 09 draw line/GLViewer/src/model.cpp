#include "model.h"

Model::Model(QOpenGLFunctions_4_5_Core* glFunc):
    m_glFunc(glFunc),
    m_VBO(QOpenGLBuffer::VertexBuffer),
    m_EBO(QOpenGLBuffer::IndexBuffer)
{
    drawingLinesholder = new DrawingLinesHolder(glFunc);
    drawingViewerElemHolder = new DrawingViewerElemHolder(glFunc);
    drawingViewerElemHolder->setup();
}

Model::~Model()
{
    m_VAO.destroy();
    m_VBO.destroy();
    m_EBO.destroy();
}

void Model::Draw(QOpenGLShaderProgram &shader)
{
    if (!m_VAO.isCreated())
        m_VAO.create();
    else
        m_VAO.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_VAO);

    if (!m_VBO.isCreated())
        m_VBO.create();
    else
        m_VBO.release();
    if (!m_EBO.isCreated())
        m_EBO.create();
    else
        m_EBO.release();

    m_VBO.bind();
    m_VBO.allocate(vertices, sizeof(vertices));

    m_EBO.bind();
    m_EBO.allocate(indices, sizeof(indices));

    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    shader.setUniformValue("objectColor", 1.f, 0.2f, 0.2f, 1.f);

    QOpenGLVertexArrayObject::Binder bind(&m_VAO);
    m_glFunc->glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}

void Model::Draw2(QOpenGLShaderProgram& shader)
{
    m_glFunc->glLineWidth(2.0f);

    //  work plane
    //  由于工作平面有透明度，最后绘制
    drawingViewerElemHolder->drawWorkPlane(shader);

    Draw(shader);

    drawingLinesholder->DrawLines(shader, ViewerSetting::previewData);
    drawingLinesholder->DrawSelectedLines(shader);
}

void Model::DrawViewElement(QOpenGLShaderProgram& shader, const string& name)
{
    drawingViewerElemHolder->drawViewElement(shader, name);
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


const BodyInfo* Model::GetViewCubeBody()
{
    return drawingViewerElemHolder->GetViewCubeBody();
}

