#include "glview.h"
#include <QMessageBox>

GLView::GLView(QWidget *parent)
    : QOpenGLWidget{parent}
{
    //  y-up to z-up
    m_modelMatrix = QMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);//(1.0f);
    m_modelMatrix = m_modelMatrix.transposed();

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

    initShader(m_lightShader);
    initTextureShader(m_textureShader);

    m_model = new Model(this);
    m_camera.lastFrame = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;

}

void GLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    m_camera.SCR_WIDTH = w;
    m_camera.SCR_HEIGHT = h;
}

void GLView::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float currentFrame = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    m_camera.deltaTime = currentFrame - m_camera.lastFrame;
    m_camera.lastFrame = currentFrame;

    m_lightShader.bind();

    m_projectionMat.setToIdentity();
    m_projectionMat.perspective(/*qDegreesToRadians*/(m_camera.Zoom), (float)m_camera.SCR_WIDTH / (float)m_camera.SCR_HEIGHT, 0.1f, 100.0f);

    m_viewMat = m_camera.GetViewMatrix();

    m_lightShader.setUniformValue("projection", m_projectionMat);
    m_lightShader.setUniformValue("view", m_viewMat);
    m_lightShader.setUniformValue("model", m_modelMatrix);

    m_model->Draw2(m_lightShader);
    m_lightShader.release();

    m_textureShader.bind();

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
}

void GLView::initShader(QOpenGLShaderProgram& shader)
{
    bool result = true;
    result = shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/1.model_loading.vert");
    if (!result) {
        qDebug() << shader.log();
    }
    result = shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/1.model_loading.frag");
    if (!result) {
        qDebug() << shader.log();
    }
    result = shader.link();
    if (!result) {
        qDebug() << shader.log();
    }
}

void GLView::initTextureShader(QOpenGLShaderProgram& shader)
{
    bool result = true;
    result = shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/1.model_loading_texture.vert");
    if (!result) {
        qDebug() << shader.log();
    }
    result = shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/1.model_loading_texture.frag");
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

    if (m_camera.handle(e))
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
}

/////////////////////////////////////////////

void GLView::drawLine()
{
    QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
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

