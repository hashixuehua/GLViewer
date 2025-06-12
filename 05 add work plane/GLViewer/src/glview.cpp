#include "glview.h"

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

bool GLView::event(QEvent* e)
{
    makeCurrent();

    if (m_camera.handle(e))
        update();

    doneCurrent();
    return QWidget::event(e);
}

