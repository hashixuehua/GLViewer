#ifndef GLVIEW_H
#define GLVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QLineEdit>

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

    void UpdateParaInputPos(int px, int py);
    void OnCommandExcuted(bool onlyMove = true);
    void OnCommandStart();

private:
    void initShader(QOpenGLShaderProgram& shader);
    void initTextureShader(QOpenGLShaderProgram& shader);

    void getViewCubeProjectOffset(QMatrix4x4& offViewCube);
    void getViewCubeProjectOffsetStio(float& xStio, float& yStio);

    int ClickedViewCube(int px, int py);

    void onMouseLeftPress(QMouseEvent* event);
    void onKeyPress(QKeyEvent* event);
    void onMouseMove(QMouseEvent* event);
    void onMouseButtonRelease();

    bool tryToExcuteCommandOnValidKeyPress();

public:
    std::function<void(const string&)> OnStatusMessageChanged;

private:
    //  current mouse pos
    int currentMousePos[2];

    //  command
    CommandPara* m_commandPara;

    //  input
    QLineEdit* paraInput;

private:
    Model* m_model = nullptr;
    Camera m_camera = Camera(this, QVector3D(0.0f, 6.0f, 10.0f));

    QMatrix4x4 m_modelMatrix;  //y-up to z-up
    QMatrix4x4 m_viewMat;
    QMatrix4x4 m_projectionMat;
    QMatrix4x4 m_projection4ViewCube;

    // 着色器变量
    QOpenGLShaderProgram m_lightShader;
    QOpenGLShaderProgram m_textureShader;

private:
    void drawCurve(ViewerStatus status, int drawType = -1);
    bool getRayIntersection(int px, int py, QVector3D& hitPt);

public slots:
    void drawLine();
    void updatePreviewNextPoint(QMouseEvent* event);


signals:
};

#endif // GLVIEW_H
