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
    void AddVertexToLines(const CommandPara& para);
    void SetPreviewNextPoint(double x, double y, double z);
    bool isLinePreviewValid();

    void getOrthogonalityPoint(Vector3f& cur);

    void ClearInvalidData();

    //
    const BodyInfo* GetViewCubeBody();

private:
    void drawWorkPlane(QOpenGLShaderProgram& shader);
    void GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh);

    void setupWorkPlaneMesh();
    void setupViewCube();
    void setupMouse();

    void addCompletePreviewDataToLines();
    void ExcuteAction(const list<Line>& lstLine);

    shared_ptr<Mesh> ConvertMesh(const TriangleMesh& mesh);

public:
    //  preview
    //Vector3f* previewNextPt;
    PeviewData previewData;

    //  捕捉
    bool snapMode;

    //  正交模式
    bool orthogonalityMode;

    //  工作平面
    Vector3f currentWorkPlPoint = Vector3f::Zero;
    Vector3f currentWorkPlNormal = Vector3f::BasicZ;

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

    //  mesh for viewer
    map<string, MeshInfo> mapName2VMesh;
    map<string, BodyInfo> mapName2VBody;

    DrawingLinesHolder* drawingLinesholder;

};

#endif // MODEL_H
