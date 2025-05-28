#ifndef MODEL_H
#define MODEL_H
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_4_5_Core>

#include <CGLib/TriangleMesh.h>

#include "CGLibUtils.h"
#include "ViewerUtils.h"
#include "ViewerSetting.h"

#include "DrawingLinesHolder.h"
#include "DrawingViewerElemHolder.h"

using namespace std;
using namespace CGUTILS;

class Model
{
public:
    Model(QOpenGLFunctions_4_5_Core* glFunc);
    ~Model();

    void Draw(QOpenGLShaderProgram& shader);
    void Draw2(QOpenGLShaderProgram& shader);
    void DrawViewElement(QOpenGLShaderProgram& shader, const string& name);

    //  for curve drawing
    void BeginToDrawLines(GLCurveType curveType, int drawType = -1);
    void EndOfDrawingLine();

    void AddVertexToLines(const CommandPara& para);
    void SetPreviewNextPoint(double x, double y, double z);
    bool IsLinePreviewValid();

    void GetOrthogonalityPoint(Vector3f& cur);

    //  for switch view by viewCube
    const BodyInfo* GetViewCubeBody();

private:
    void addCompletePreviewDataToLines();
    void excuteAction(const list<Line>& lstLine);

private:
    // 顶点的数据
    float vertices[18] = {
        -0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f,
        0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f,
        0.0f,  0.5f, 1.f, 0.f, 0.f, 1.f
    };

    unsigned int indices[3]{ 0, 1, 2 };

    QOpenGLFunctions_4_5_Core* m_glFunc;

    QOpenGLVertexArrayObject m_VAO;
    QOpenGLBuffer m_VBO, m_EBO;

    DrawingLinesHolder* drawingLinesholder;
    DrawingViewerElemHolder* drawingViewerElemHolder;

};

#endif // MODEL_H
