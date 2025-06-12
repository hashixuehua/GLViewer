#ifndef GLVIEW_H
#define GLVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "model.h"
#include "camera.h"

class GLView : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit GLView(QWidget *parent = nullptr);
    ~GLView();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    virtual bool event(QEvent* e);

private:
    void initShader(QOpenGLShaderProgram& shader);

private:
    Model* m_model = nullptr;
    Camera m_camera = Camera(this, QVector3D(0.0f, 6.0f, 10.0f));

    QMatrix4x4 m_modelMatrix;  //y-up to z-up
    QMatrix4x4 m_viewMat;
    QMatrix4x4 m_projectionMat;

    // 着色器变量
    QOpenGLShaderProgram m_lightShader;

signals:
};

#endif // GLVIEW_H
