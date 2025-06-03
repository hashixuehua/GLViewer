#pragma once
#include <CGLib/TriangleMesh.h>

#include "CGLibUtils.h"
#include "ViewerUtils.h"
#include "ViewerSetting.h"

using namespace std;
using namespace CGUTILS;

class DrawingViewerElemHolder
{
public:
    DrawingViewerElemHolder(QOpenGLFunctions_4_5_Core* glFunc);
    ~DrawingViewerElemHolder();

    void setup();

    void drawWorkPlane(QOpenGLShaderProgram& shader);
    void drawViewElement(QOpenGLShaderProgram& shader, const string& name);
    void drawRectSelect(QOpenGLShaderProgram& shader, const vector<Vector3f>& rectData);

    const BodyInfo* GetViewCubeBody();

    void setCurrentWorkPlane(const Vector3f& planePt, const Vector3f& planeNormal, float offset);

private:
    void setupWorkPlaneMesh();
    void setupViewCube();
    void setupMouse();
    void setupSnap();
    void setupRectSelect();//方式一
    void setupRectSelect(const vector<Vector3f>& rectData);//方式二

    void GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh);

private:
    QOpenGLFunctions_4_5_Core* m_glFunc;

    //  for rect select
    QOpenGLVertexArrayObject VAORectSelect;
    QOpenGLBuffer VBORectSelect, EBORectSelect;

    vector<unsigned int> rectSelectIndices;

    //  mesh for viewer
    map<string, MeshInfo> mapName2VMesh;
    map<string, BodyInfo> mapName2VBody;

};