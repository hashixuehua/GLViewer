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
#include "SelectionFramebuffer.h"
#include "MSAAFramebuffer.h"
#include "LayerSettingDialog.h"
#include "RenderSettingDialog.h"

using namespace SelectionRender;
using namespace MSAARender;

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
    void initShader(QOpenGLShaderProgram& shader, const QString& vertShaderFile, const QString& fragShaderFile);

    void getViewCubeProjectOffset(QMatrix4x4& offViewCube);
    void getViewCubeProjectOffsetStio(float& xStio, float& yStio);

    int ClickedViewCube(int px, int py);

    void onMouseLeftPress(QMouseEvent* event);
    void onKeyPress(QKeyEvent* event);
    void onMouseMove(QMouseEvent* event);
    void onMouseButtonRelease();

    bool tryToExcuteCommandOnValidKeyPress();

    void setCurWorkPlane(int px, int py);

public:
    std::function<void(const string&)> OnStatusMessageChanged;

    void OnLayerRemoved(const LayerData& layerData);
    void OnLayerColorChanged(const LayerData& layerData);

    void OnMSAAModeChanged(bool enable, int mode);
    void OnEdgeLineModeChanged(bool showEdgeLine, bool PPAA);

private:
    //  current mouse pos
    int currentMousePos[2];

    //  command
    CommandPara* m_commandPara;

    //  input
    QLineEdit* paraInput;

    //  layerSetting
    LayerSettingDialog* layerSettingDlg;

    //  RenderSetting
    RenderSettingDialog* renderSettingDlg;

private:
    Model* m_model = nullptr;
    Camera m_camera = Camera(this, QVector3D(0.0f, 6.0f, 10.0f));

    QMatrix4x4 m_modelMatrix;  //y-up to z-up
    QMatrix4x4 m_viewMat;
    QMatrix4x4 m_projectionMat;
    QMatrix4x4 m_projection4ViewCube;

    QVector3D m_lightPos;

    // 着色器变量
    QOpenGLShaderProgram m_lightShader;
    QOpenGLShaderProgram m_textureShader;
    QOpenGLShaderProgram m_selectShader;
    QOpenGLShaderProgram m_singleColorShader;

    SelectionFramebuffer* mSelectionFramebuffer;
    MSAAFramebuffer* mMSAAFramebuffer;

private:
    void drawCurve(ViewerStatus status, int drawType = -1);
    bool getRayIntersection(int px, int py, QVector3D& hitPt);

    void SetSelectedElement(int elementId, bool addMode);
    void TrySelectComponent(float x, float y, bool addMode);

    void DrawSelectionFrame();
    void SelectionResize(int width, int height);
    void MSAAResize(int width, int height);
    CurveQueryInfo Query(const QPoint& queryPoint);

    void DrawSelected(QOpenGLShaderProgram& shader);
    void DrawEdgeLineWithAntiAliasing(QOpenGLShaderProgram& shader);

public slots:
    void drawLine();
    void drawArc();
    void drawArc2(void);
    void drawCircle(void);
    void drawRectangle(void);
    void drawImage(void);

    //  read and write
    void readLinesFromFile(void);
    void saveLinesToFile(void);

    void boxSelect(void);
    void copy(void);
    void deleteSelected(void);
    void showOrHideModel(void);
    void showOrHideLines(void);

    void setWorkPlane(void);
    void layerSetting(void);
    void wireFrame(void);
    void renderSetting(void);

    void updatePreviewNextPoint(QMouseEvent* event);


signals:
};

#endif // GLVIEW_H
