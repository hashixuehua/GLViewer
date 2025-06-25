#include "glview.h"
#include <QMessageBox>
#include <QFileDialog>
#include "Command.h"

GLView::GLView(QWidget *parent)
    : QOpenGLWidget{ parent }, currentMousePos{0}, m_lightPos(5.f, 20.f, 5.0f), mSelectionFramebuffer(nullptr), mMSAAFramebuffer(nullptr)
{
    // set focus policy to threat QContextMenuEvent from keyboard  
    setFocusPolicy(Qt::StrongFocus);

    // Enable the mouse tracking, by default the mouse tracking is disabled.
    setMouseTracking(true);

    //  y-up to z-up
    m_modelMatrix = ViewerSetting::matrixYup2Zup;

    m_commandPara = new CommandPara();

    //  layer
    auto f1 = std::bind(&GLView::OnLayerRemoved, this, std::placeholders::_1);
    auto f2 = std::bind(&GLView::OnLayerColorChanged, this, std::placeholders::_1);
    LayerCache::OnLayerRemoved = f1;
    LayerCache::OnLayerColorChanged = f2;
    layerSettingDlg = new LayerSettingDialog(this);

    paraInput = new QLineEdit(this);
    paraInput->setFixedSize(80, 16);
    paraInput->hide();
    paraInput->setFocus();

}

GLView::~GLView()
{
    makeCurrent();
    delete m_model;

    doneCurrent();
}

void GLView::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    initShader(m_lightShader, ":/shader/1.model_loading.vert", ":/shader/1.model_loading.frag");
    initShader(m_textureShader, ":/shader/1.model_loading_texture.vert", ":/shader/1.model_loading_texture.frag");
    initShader(m_selectShader, ":/shader/1.model_loading.vert", ":/shader/Selection.frag");
    initShader(m_singleColorShader, ":/shader/1.model_loading.vert", ":/shader/singleColor.frag");

    m_model = new Model(this);
    m_camera.lastFrame = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    auto& defaultLayer = LayerCache::getDefaultLayer();

}

void GLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    m_camera.SCR_WIDTH = w;
    m_camera.SCR_HEIGHT = h;
    SelectionResize(w, h);
    MSAAResize(ViewerSetting::devicePixelRatio * w, ViewerSetting::devicePixelRatio * h);
}

void GLView::paintGL()
{
    // 1. draw scene as normal in multisampled buffers
    mMSAAFramebuffer->EnableWriting();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);

    float currentFrame = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    m_camera.deltaTime = currentFrame - m_camera.lastFrame;
    m_camera.lastFrame = currentFrame;

    m_lightShader.bind();
    m_lightShader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_lightShader.setUniformValue("lightPos", m_lightPos);

    m_projectionMat.setToIdentity();
    m_projectionMat.perspective(/*qDegreesToRadians*/(m_camera.Zoom), (float)m_camera.SCR_WIDTH / (float)m_camera.SCR_HEIGHT, 0.1f, 100.0f);
    m_viewMat = m_camera.GetViewMatrix();

    m_lightShader.setUniformValue("projection", m_projectionMat);
    m_lightShader.setUniformValue("view", m_viewMat);
    m_lightShader.setUniformValue("model", m_modelMatrix);

    m_model->Draw2(m_lightShader);

    //  copy
    if (ViewerSetting::IsCopyElementsPreviewValid())
    {
        QMatrix4x4 off;
        off.translate(ViewerSetting::GetCopyOffset());
        m_lightShader.setUniformValue("model", m_modelMatrix * off);
        m_model->DrawCopyElementsPreview(m_lightShader);
        m_lightShader.setUniformValue("model", m_modelMatrix/*QMatrix4x4()*/);
    }

    //  rectSelect
    if (ViewerSetting::IsRectSelectValid())
    {
        vector<Vector3f> rectData;
        ViewerUtils::getWCSRectSelect(*ViewerSetting::rectSelectStart, *ViewerSetting::rectSelectEnd, 0.0, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, m_modelMatrix.inverted(), m_camera.GetViewMatrix().inverted(), m_projectionMat.inverted(), rectData);

        //  方式一
        m_lightShader.setUniformValue("model", m_modelMatrix);
        m_model->DrawRectSelect(m_lightShader, rectData);

        //  方式二
        //Transform trs = ViewerUtils::getRectSelectMatrix(rectData);
        //QMatrix4x4 mat;
        //CGLibUtils::Transform2QMatrix4x4(trs, mat);
        //
        //m_lightShader.setUniformValue("model", m_modelMatrix * mat);
        //m_model->DrawViewElement(m_lightShader, ViewerCache::rectSelect);
    }

    //  snap label
    if (ViewerSetting::previewData.previewNextPt)
    {
        QMatrix4x4 modelSnap;
        modelSnap.translate(QVector3D(ViewerSetting::previewData.previewNextPt->X, ViewerSetting::previewData.previewNextPt->Y, ViewerSetting::previewData.previewNextPt->Z));
        if (!ViewerSetting::currentWorkPlNormal.IsEqual(Vector3f::BasicZ, 1e-3))
        {
            Vector3f tNor = Vector3f::BasicZ.CrossProduct(ViewerSetting::currentWorkPlNormal);
            double dAngle = qRadiansToDegrees(Vector3f::BasicZ.Angle(ViewerSetting::currentWorkPlNormal));
            modelSnap.rotate(dAngle, QVector3D(tNor.X, tNor.Y, tNor.Z));
        }

        //modelSnap = m_modelMatrix * modelSnap;
        m_lightShader.setUniformValue("model", m_modelMatrix * modelSnap);
        m_model->DrawViewElement(m_lightShader, ViewerCache::snapLabel);
    }

    m_lightShader.release();
    m_textureShader.bind();

    //  image
    m_textureShader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_textureShader.setUniformValue("lightPos", m_lightPos);
    m_textureShader.setUniformValue("projection", m_projectionMat);
    m_textureShader.setUniformValue("view", m_viewMat);
    m_textureShader.setUniformValue("model", m_modelMatrix);

    //m_model->DrawImage(m_textureShader, "D:/work/industrial_software_video/image/公路上的姐妹/girl.png", 500.0, 500.0, Vector3f::BasicZ, Vector3f(0.0, -1000.0, 0.0), 0.0);
    m_model->DrawImage(m_textureShader);

    //  尽量使viewer元素的深度值小(通过缩小元素尺寸+将观察点设置很近)，便于一直绘制
    //  mouse
    float mousePX, mousePY;
    double mouseDepth;
    ViewerUtils::worldToScreen(m_camera.mousePos4Rotate, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, false, m_modelMatrix, m_viewMat, m_projectionMat, mousePX, mousePY, mouseDepth);
    float pt_x = ((float)mousePX / m_camera.SCR_WIDTH) * 2.f - 1.f;
    float pt_y = -((float)mousePY / m_camera.SCR_HEIGHT) * 2.f + 1.f;
    QMatrix4x4 offMouseLabel;
    offMouseLabel.translate(QVector3D(pt_x, pt_y, 0.0));

    m_projection4ViewCube.setToIdentity();
    m_projection4ViewCube.perspective(45.0f, (float)m_camera.SCR_WIDTH / (float)m_camera.SCR_HEIGHT, 0.1f, 100.0f);

    QMatrix4x4 modelMouse;
    modelMouse.translate(m_camera.mousePos4Rotate);
    m_textureShader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_textureShader.setUniformValue("lightPos", QVector3D(0.f, 1.f, 0.f));
    m_textureShader.setUniformValue("projection", offMouseLabel * m_projection4ViewCube);
    m_textureShader.setUniformValue("view", m_camera.GetViewMatrix4MouseLabel());
    m_textureShader.setUniformValue("model", m_modelMatrix);
    m_model->DrawViewElement(m_textureShader, ViewerCache::mouseLabel);

    // view cube
    QMatrix4x4 viewMatNoTrans = m_camera.GetViewMatrix4VieweCube();
    QMatrix4x4 offViewCube;
    getViewCubeProjectOffset(offViewCube);

    QVector3D posLtCube = (-10 * m_camera.Front);
    QVector4D lightCube = /*m_modelMatrix.inverted() **/ QVector4D(posLtCube.x(), posLtCube.y(), posLtCube.z(), 1.0);
    m_textureShader.setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_textureShader.setUniformValue("lightPos", lightCube.toVector3D());
    m_textureShader.setUniformValue("projection", offViewCube * m_projection4ViewCube);
    m_textureShader.setUniformValue("view", viewMatNoTrans);
    m_textureShader.setUniformValue("model", m_modelMatrix);

    m_model->DrawViewElement(m_textureShader, ViewerCache::viewCube);

    //qDebug() << m_textureShader.log();
    m_textureShader.release();

    //  draw selected components
    DrawSelected();

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. now blit multisampled buffer(s) to defaultbuffer
    mMSAAFramebuffer->BlitFrameBuffer(defaultFramebufferObject());
    
    //qDebug() << defaultFramebufferObject();
    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    //glDrawBuffer(GL_BACK);

    //  selection info
    DrawSelectionFrame();
}

void GLView::initShader(QOpenGLShaderProgram& shader, const QString& vertShaderFile, const QString& fragShaderFile)
{
    bool result = true;
    result = shader.addShaderFromSourceFile(QOpenGLShader::Vertex, vertShaderFile);
    if (!result) {
        qDebug() << shader.log();
    }
    result = shader.addShaderFromSourceFile(QOpenGLShader::Fragment, fragShaderFile);
    if (!result) {
        qDebug() << shader.log();
    }
    result = shader.link();
    if (!result) {
        qDebug() << shader.log();
    }
}

bool GLView::event(QEvent* e)
{
    makeCurrent();

    if (e->type() == QEvent::MouseButtonPress)
    {
        auto event = static_cast<QMouseEvent*>(e);
        if (event->button() == Qt::LeftButton)
        {
            onMouseLeftPress(event);
        }
    }
    else if (e->type() == QEvent::KeyPress)
    {
        auto event = static_cast<QKeyEvent*>(e);
        onKeyPress(event);
    }
    else if (e->type() == QEvent::MouseMove)
    {
        auto event = static_cast<QMouseEvent*>(e);
        onMouseMove(event);
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        onMouseButtonRelease();
    }

    if (!m_model || !ViewerSetting::isSelectMode)
    {
        if (m_camera.handle(e))
            update();
    }
    else
        update();

    doneCurrent();
    return QWidget::event(e);
}

void GLView::onMouseLeftPress(QMouseEvent* event)
{
    //  check if clicked the view cube
    int flag = ClickedViewCube(event->x(), event->y());
    if (flag != -1)
    {
        m_camera.FitView(flag);
        return;
    }

    int nStatus = ViewerSetting::IsStatusNeedDraw(ViewerSetting::viewerStatus);
    if (nStatus != -1 && nStatus != 4)
    {
        if (!ViewerSetting::previewData.previewNextPt)  //在连续点击的情况下（有时候mouse mouve还没有触发），previewNextPt会出现为null的情况
            updatePreviewNextPoint(event);
        if (!ViewerSetting::previewData.previewNextPt)
        {
            QMessageBox::warning(this, "Warning!", "Please click on current workplane, you can rotate the view by moving on pressing left mouse.");
            return;
        }

        m_commandPara->Init();
        if (ViewerCommand::OnMouseLeftDown("", ViewerSetting::previewData.previewNextPt, nullptr, *m_commandPara))
        {
            if (m_commandPara->vType != -1)
                m_model->AddVertexToLines(*m_commandPara);

            OnCommandExcuted();
        }
    }
    else
    {
        TrySelectComponent(event->x()/* * ViewerSetting::devicePixelRatio*/, event->y()/* * ViewerSetting::devicePixelRatio*/, QGuiApplication::keyboardModifiers() == Qt::ControlModifier);
        if (nStatus == 4)  //ViewerSetting::viewerStatus == ViewerStatus::SetWorkPlane
            setCurWorkPlane(event->x(), event->y());
    }
}

void GLView::setCurWorkPlane(int px, int py)
{
    //  还需要传递射线
    QVector3D rayOri, rayEnd;
    ViewerUtils::getPickRay(px, py, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, m_camera.Position, m_modelMatrix.inverted(), m_camera.GetViewMatrix().inverted(), m_projectionMat.inverted(), rayOri, rayEnd);

    Vector3f cRayOri(1000.0 * rayOri.x(), 1000.0 * rayOri.y(), 1000.0 * rayOri.z());
    Vector3f cRayEnd(1000.0 * rayEnd.x(), 1000.0 * rayEnd.y(), 1000.0 * rayEnd.z());
    m_model->SetWorkPlane(cRayOri, cRayEnd);
}

void GLView::onKeyPress(QKeyEvent* event)
{
    int eKey = event->key();
    if (eKey == Qt::Key::Key_Escape)
    {
        ViewerSetting::viewerStatus = ViewerStatus::Unknown;
        ViewerSetting::previewData.clear();
        m_model->EndOfDrawingLine();

        ViewerSetting::ClearRectSelect(false);
        ViewerSetting::ClearCopyImData();

        m_commandPara->Init();
        ViewerCommand::Init();

        OnCommandExcuted(false);
    }
    else if (eKey == Qt::Key::Key_F8)
    {
        if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
            ViewerSetting::orthogonalityMode = !ViewerSetting::orthogonalityMode;
    }
    else if (eKey == Qt::Key::Key_F9)
    {
        if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
            ViewerSetting::snapMode = !ViewerSetting::snapMode;
    }
    else if (eKey == Qt::Key::Key_Enter || eKey == Qt::Key::Key_Return)
    {
        tryToExcuteCommandOnValidKeyPress();
    }
}

bool GLView::tryToExcuteCommandOnValidKeyPress()
{
    if (paraInput->text().isEmpty())
        return false;

    int nStatus = ViewerSetting::IsStatusNeedDraw(ViewerSetting::viewerStatus);
    if (nStatus != -1 && nStatus != 4 && m_model->IsLinePreviewValid())
    {
        QVector3D hitPt;
        if (getRayIntersection(currentMousePos[0], currentMousePos[1], hitPt))
        {
            Vector3f pt(hitPt.x(), hitPt.y(), hitPt.z());
            m_model->GetOrthogonalityPoint(pt);

            //auto lastVPt = m_model->previewData.curData.empty() ? nullptr : &m_model->previewData.curData.back().pValue;
            auto lastVPt = ViewerSetting::previewData.GetCurLastValidPoint();
            m_commandPara->Init();
            if (ViewerCommand::OnKeyEnterDown(paraInput->text().toStdString(), &pt, lastVPt, *m_commandPara))
            {
                if (m_commandPara->vType != -1)
                    m_model->AddVertexToLines(*m_commandPara);

                OnCommandExcuted();
                return true;
            }
        }
    }

    return false;
}

void GLView::onMouseMove(QMouseEvent* event)
{
    currentMousePos[0] = event->x();
    currentMousePos[1] = event->y();

    int nStatus = ViewerSetting::IsStatusNeedDraw(ViewerSetting::viewerStatus);
    if (nStatus != -1 && m_model->IsLinePreviewValid())
    {
        UpdateParaInputPos(event->x(), event->y());

        updatePreviewNextPoint(event);
    }
    else if (ViewerSetting::isSelectMode && (event->buttons() & Qt::LeftButton))
    {
        //  rect select
        ViewerSetting::UpdateRectSelect(event->x(), event->y());
    }
}

void GLView::onMouseButtonRelease()
{
    if (ViewerSetting::IsRectSelectValid())
    {
        bool ctrlPressed = QGuiApplication::keyboardModifiers() == Qt::ControlModifier;
        if (!ctrlPressed)
            m_model->ClearSelected();

        Body rectData;
        ViewerUtils::getWCSRectSelectBox(*ViewerSetting::rectSelectStart, *ViewerSetting::rectSelectEnd, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, m_modelMatrix.inverted(), m_camera.GetViewMatrix().inverted(), m_projectionMat.inverted(), rectData);
        m_model->UpdateSelectInfo(rectData);

        ViewerSetting::ClearRectSelect(ViewerSetting::isSelectMode);
    }
}

/////////////////////////////////////////////

GLCurveType getCurveType(ViewerStatus status)
{
    if (status == ViewerStatus::DrawingLines)
        return GLCurveType::GLLine;
    if (status == ViewerStatus::DrawingArc)
        return GLCurveType::GLArc;
    if (status == ViewerStatus::DrawingCircle)
        return GLCurveType::GLCircle;
    if (status == ViewerStatus::DrawingRectangle)
        return GLCurveType::GLRectangle;
    if (status == ViewerStatus::Unknown)
        return GLCurveType::GLUnknown;
    if (status == ViewerStatus::Copy ||
        status == ViewerStatus::DrawingImage)
        return GLCurveType::GLTemp;
    if (status == ViewerStatus::CSphere ||
        status == ViewerStatus::SetWorkPlane)
        return GLCurveType::GLPoint;
    return GLCurveType::GLUnknown;
}

void GLView::drawLine()
{
    //QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    makeCurrent();
    drawCurve(ViewerStatus::DrawingLines, -1);
    doneCurrent();
    update();
}

void GLView::drawArc()
{
    drawCurve(ViewerStatus::DrawingArc, 0);
}

void GLView::drawArc2(void)
{
    drawCurve(ViewerStatus::DrawingArc, 1);
}

void GLView::drawCircle(void)
{
    drawCurve(ViewerStatus::DrawingCircle, 0);
}

void GLView::drawRectangle(void)
{
    drawCurve(ViewerStatus::DrawingRectangle, 0);
}

void GLView::drawImage(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open a image file."), "D:/", tr("png file(*.png);;All files(*.*)"));
    if (fileName.isEmpty())
    {
        //QMessageBox::warning(this, "Warning!", "Failed to open the json file!");
        return;
    }
    m_model->SetImageDrawingFile(fileName);

    drawCurve(ViewerStatus::DrawingImage, -1);
}

//  read and write
void GLView::readLinesFromFile(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open a file."), "D:/", tr("json file(*.json *txt);;All files(*.*)"));
    if (fileName.isEmpty())
    {
        //QMessageBox::warning(this, "Warning!", "Failed to open the json file!");
        return;
    }

    m_model->readLinesFromFile(fileName.toStdWString());

    //  update layer
    layerSettingDlg->UpdateLayerData();
}

void GLView::saveLinesToFile(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("save to file."), "D:/untitled.json", tr("json file(*.json *txt);;All files(*.*)"));
    if (fileName.isEmpty())
    {
        //QMessageBox::warning(this, "Warning!", "Failed to open the json file!");
        return;
    }

    m_model->saveLinesToFile(fileName.toStdWString());
}

void GLView::boxSelect(void)
{
    ViewerSetting::isSelectMode = true;
}

void GLView::copy(void)
{
    drawCurve(ViewerStatus::Copy, -1);
}

void GLView::deleteSelected(void)
{
    m_model->DeleteSelectedElements();
}

void GLView::showOrHideModel(void)
{
    ViewerSetting::SwitchModelShownType();
    update();
}

void GLView::showOrHideLines(void)
{
    ViewerSetting::SwitchLinesShownType();
    update();
}

void GLView::setWorkPlane(void)
{
    //  （1）选择构件得到目标构件（2）从目标构件中搜索对应面（3）设置为工作平面
    drawCurve(ViewerStatus::SetWorkPlane, -1);
    m_model->ClearSelected();
}

//  layer
void GLView::layerSetting(void)
{
    layerSettingDlg->exec();
}

void GLView::wireFrame(void)
{
    ViewerSetting::SwitchWireframeMode();
    update();
}

void GLView::OnLayerRemoved(const LayerData& layerData)
{
    m_model->DeleteLinesByRemovedLayer(layerData);
    update();
}

void GLView::OnLayerColorChanged(const LayerData& layerData)
{
    update();
}

void GLView::drawCurve(ViewerStatus status, int drawType /*= -1*/)
{
    ViewerSetting::viewerStatus = status;
    //if (m_model->lines.at(m_model->norDefualt).empty())
    //    throw exception("draw curve error.");
    m_model->BeginToDrawLines(getCurveType(status), drawType);

    ViewerCommand::ActiveCommand(status, drawType);
    OnStatusMessageChanged(ViewerCommand::GetCommandStatus());

    OnCommandStart();
}

void GLView::updatePreviewNextPoint(QMouseEvent* event)
{
    QVector3D hitPt;
    if (getRayIntersection(event->x(), event->y(), hitPt))
    {
        //  test
        //if (hitPt.z() != 0.0f)
        //    throw exception("test.");

        Vector3f pt(hitPt.x(), hitPt.y(), hitPt.z());
        m_model->GetOrthogonalityPoint(pt);
        m_model->SetPreviewNextPoint(pt.X, pt.Y, pt.Z);
    }
}

bool GLView::getRayIntersection(int px, int py, QVector3D& hitPt)
{
    QVector3D rayOri, rayEnd;
    ViewerUtils::getPickRay(px, py, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, m_camera.Position, m_modelMatrix.inverted(), m_camera.GetViewMatrix().inverted(), m_projectionMat.inverted(), rayOri, rayEnd);

    //CGUTILS::Plane plane(Vector3d::Zero, Vector3d::BasicZ);
    CGUTILS::Plane plane(ViewerSetting::currentWorkPlPoint / 1000.0, ViewerSetting::currentWorkPlNormal);

    return ViewerUtils::getIntersection(rayOri/*m_camera.Position*/, rayEnd, plane, hitPt);
}

/////////////////////////////////////////////

int GLView::ClickedViewCube(int px, int py)
{
    float xStio, yStio;
    getViewCubeProjectOffsetStio(xStio, yStio);

    px = px - m_camera.SCR_WIDTH * 0.5 * xStio;
    py = py + m_camera.SCR_HEIGHT * 0.5 * yStio;

    QVector3D rayOri4VCube, rayEnd4VCube;
    QVector3D cmrPos4VCube = m_camera.GetCameraPos4ViewCube();
    ViewerUtils::getPickRay(px, py, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, cmrPos4VCube/*m_camera.Position*/, m_modelMatrix.inverted(), m_camera.GetViewMatrix4VieweCube().inverted(), m_projectionMat.inverted(), rayOri4VCube, rayEnd4VCube);

    Vector3f planePt, planeNormal;
    if (!CGLibUtils::getClickHittedPlane(Vector3f(1000.0 * rayOri4VCube.x(), 1000.0 * rayOri4VCube.y(), 1000.0 * rayOri4VCube.z()), Vector3f(1000.0 * rayEnd4VCube.x(), 1000.0 * rayEnd4VCube.y(), 1000.0 * rayEnd4VCube.z()), *m_model->GetViewCubeBody(), planePt, planeNormal))
        return -1;

    if (planeNormal.IsEqual(Vector3f::BasicX))
        return 2;
    else if (planeNormal.IsEqual(-Vector3f::BasicX))
        return 4;
    else if (planeNormal.IsEqual(Vector3f::BasicY))
        return 3;
    else if (planeNormal.IsEqual(-Vector3f::BasicY))
        return 1;
    else if (planeNormal.IsEqual(Vector3f::BasicZ))
        return 5;
    else if (planeNormal.IsEqual(-Vector3f::BasicZ))
        return 0;

    throw exception("error.");
}

void GLView::getViewCubeProjectOffsetStio(float& xStio, float& yStio)
{
    xStio = 0.85;
    yStio = 0.7;
}

void GLView::getViewCubeProjectOffset(QMatrix4x4& offViewCube)
{
    float xStio, yStio;
    getViewCubeProjectOffsetStio(xStio, yStio);

    QVector3D rU;
    //screenToWord(m_camera.SCR_WIDTH - 60, 60, 0.8, { 0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT }, m_modelMatrix.inverted(), viewMatNoTrans.inverted(), m_projection4ViewCube.inverted(), rU);
    rU.setX(xStio);
    rU.setY(yStio);

    offViewCube.translate(rU);
    //offViewCube = m_modelMatrix * offViewCube;
}

void GLView::UpdateParaInputPos(int px, int py)
{
    paraInput->move(px + 10, py);
}

void GLView::OnCommandExcuted(bool onlyMove /* = true*/)
{
    OnStatusMessageChanged(ViewerCommand::GetCommandStatus());
    paraInput->setText("");

    if (onlyMove)
    {
        paraInput->setFocus();
        //paraInput->move(-100, -100);
    }
    else
    {
        setFocus();
        paraInput->hide();
    }
}

void GLView::OnCommandStart()
{
    paraInput->setText("");
    paraInput->show();
    paraInput->setFocus();
}

//////////////////for selection//////////////////////

void GLView::SetSelectedElement(int elementId, bool addMode)
{
    m_model->SetSelected(elementId, addMode);
}

void GLView::TrySelectComponent(float x, float y, bool addMode)
{
    CurveQueryInfo info = Query(QPoint(x, y));

    if (info.result == 1)
    {
        qDebug() << "selected: " << info.index;

        SetSelectedElement(info.index, addMode);
        //SetSelectedCurve(mCurveContainer->GetCurve(info.index));
        //update();
    }
    else
    {
        m_model->ClearSelected();
        //SetSelectedCurve(nullptr);
    }
}

void GLView::DrawSelectionFrame()
{
    //mSelectionFramebuffer->Clear();
    //mSelectionFramebuffer->Bind();

    mSelectionFramebuffer->EnableWriting();

    glViewport(0, 0, m_camera.SCR_WIDTH, m_camera.SCR_HEIGHT);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_selectShader.bind();

    m_selectShader.setUniformValue("projection", m_projectionMat);
    m_selectShader.setUniformValue("view", m_camera.GetViewMatrix());
    m_selectShader.setUniformValue("model", m_modelMatrix/*QMatrix4x4()*/);

    m_model->DrawSelectionInfo(m_selectShader);

    m_selectShader.release();
    mSelectionFramebuffer->DisableWriting();
}

void GLView::DrawSelected()
{
    //  draw selected
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    //glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_NEVER);

    m_singleColorShader.bind();
    m_singleColorShader.setUniformValue("projection", m_projectionMat);
    m_singleColorShader.setUniformValue("view", m_viewMat);
    m_singleColorShader.setUniformValue("model", m_modelMatrix);

    m_model->DrawSelected(m_singleColorShader);

    //  draw outline of selected
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);

    //  TODO
    // TODO use shaderSingleColor
    m_model->DrawSelected(m_singleColorShader, true);

    m_singleColorShader.release();

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

void GLView::SelectionResize(int width, int height)
{
    if (mSelectionFramebuffer)
    {
        delete mSelectionFramebuffer;
        mSelectionFramebuffer = nullptr;
    }

    mSelectionFramebuffer = new SelectionFramebuffer(width, height);
}

void GLView::MSAAResize(int width, int height)
{
    if (mMSAAFramebuffer)
    {
        delete mMSAAFramebuffer;
        mMSAAFramebuffer = nullptr;
    }

    mMSAAFramebuffer = new MSAAFramebuffer(width, height);
}

CurveQueryInfo GLView::Query(const QPoint& queryPoint)
{
    return mSelectionFramebuffer->Query(queryPoint);
}
