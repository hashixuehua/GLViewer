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

    const BodyInfo* GetViewCubeBody();

    void setCurrentWorkPlane(const Vector3f& planePt, const Vector3f& planeNormal, float offset);

private:
    void setupWorkPlaneMesh();
    void setupViewCube();
    void setupMouse();
    void setupSnap();

    void GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh);

private:
    QOpenGLFunctions_4_5_Core* m_glFunc;

    //  mesh for viewer
    map<string, MeshInfo> mapName2VMesh;
    map<string, BodyInfo> mapName2VBody;

};