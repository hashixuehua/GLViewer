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
#include "DrawingImageHolder.h"
#include "DrawingComponentHolder.h"

using namespace std;
using namespace CGUTILS;

class Model
{
public:
    Model(QOpenGLFunctions_4_5_Core* glFunc);
    ~Model();

    void Draw2(QOpenGLShaderProgram& shader);
    void DrawViewElement(QOpenGLShaderProgram& shader, const string& name);
    void DrawImage(QOpenGLShaderProgram& shader);

    void DrawSelectionInfo(QOpenGLShaderProgram& shader);

    //  for curve drawing
    void BeginToDrawLines(GLCurveType curveType, int drawType = -1);
    void EndOfDrawingLine();

    void AddVertexToLines(const CommandPara& para);
    void SetPreviewNextPoint(double x, double y, double z);
    bool IsLinePreviewValid();

    void GetOrthogonalityPoint(Vector3f& cur);

    //  for image drawing
    void SetImageDrawingFile(const QString& image);

    //  for switch view by viewCube
    const BodyInfo* GetViewCubeBody();

    //  for select
    void SetSelected(int elementId, bool addMode);
    void ClearSelected();

private:
    void addCompletePreviewDataToLines();
    void excuteAction(const list<Line>& lstLine);

private:
    QOpenGLFunctions_4_5_Core* m_glFunc;

    DrawingLinesHolder* drawingLinesholder;
    DrawingViewerElemHolder* drawingViewerElemHolder;
    DrawingImageHolder* drawingImageHolder;
    DrawingComponentHolder* drawingComponentHolder;
};

#endif // MODEL_H
