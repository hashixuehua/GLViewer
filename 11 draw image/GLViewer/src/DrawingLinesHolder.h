#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include "CGLibUtils.h"
#include "LayerData.h"
#include "ViewerSetting.h"

class DrawingLinesHolder
{
public:
    DrawingLinesHolder(QOpenGLFunctions_4_5_Core* glFunc);
    ~DrawingLinesHolder();

    ////////// draw lines
    void BeginToDrawLines(GLCurveType curveType, int drawType = -1);
    void EndOfDrawingLine();

    void pushCurveData(GLCurveType curveType, const list<Line>& lstLine);

    void DrawSelectedLines(QOpenGLShaderProgram& shader);
    void DrawLines(QOpenGLShaderProgram& shader, const PreviewData& previewData);

    void GetSnapPoint(Vector3f& result);

    ////////// update lines by data of input file
    void updateLinesByInputFileData(const vector<string>& layerName, const Vector3f& normal, const list<Line>& lstLine);

    //  TODO 抽取代码，复用
    void deleteLinesByRemovedLayer(const LayerData& layerData);
    void deleteSelectedLines();

private:
    void addLineModelNode(const Line& line);
    void addModelNode(GLCurveType cType, const list<Line>& lstLine);

    void CopySelectedData(const Vector3f& currentWorkPlNormal, const Vector3f& offset);
    void getSelectedLines(list<Line>& result, vector<string>& layerNames);
    void UpdateSelectInfo(const Body& rectData);

    void updateLinesPreview(const PreviewData& previewData, vector<float>& lineVerts, vector<unsigned int>& lineInds);
    void updateOneLine(const CurveData& ls, vector<float>& lineVerts, vector<unsigned int>& lineInds);
    void updateSelectedLines(vector<float>& lineVerts, vector<unsigned int>& lineInds);
    void updateLines(const map<PointData_1e_3, vector<CurveData*>>& linesData, vector<float>& lineVerts, vector<unsigned int>& lineInds);

    void setupSelectedLines(const vector<float>& lineVerts, const vector<unsigned int>& lineInds);
    void setupLines(const vector<float>& lineVerts, const vector<unsigned int>& lineInds);

private:
    //  lines
    map<PointData_1e_3, vector<CurveData*>> lines;
    PointData_1e_3 norDefualt = PointData_1e_3(Vector3f::BasicZ);  //for drawing lines on work plane.

    //  lines select info
    vector<CurveData*> selectedLines;

    //  node for snap
    list<ModelNode> modelNodes;

    //  
    QOpenGLVertexArrayObject VAOLines;
    QOpenGLBuffer VBOLines, EBOLines;

    vector<unsigned int> lineIndices;

    //  selected lines
    QOpenGLVertexArrayObject VAOSelectedLines;
    QOpenGLBuffer VBOSelectedLines, EBOSelectedLines;

    vector<unsigned int> selectedLineIndices;

    //opengl函数入口
    QOpenGLFunctions_4_5_Core* m_glFunc;
};