#pragma once
#include "CGLibUtils.h"

class PreviewData
{
public:
    PreviewData();

    GLCurveType curveType;
    int drawType;  //多种方式draw curve，如[起始点、终止点、弧上点]、[圆心、半径、起始弧度、终止弧度]都可画圆弧
    Vector3f wkPlaneNormal;  //工作平面法向
    vector<CommandPara> curData;
    Vector3f* previewNextPt;
    bool complete;

    //  temp data
    //vector<Vector3f> lstTempPt;

public:
    void addPoint(const Vector3f& pt);
    void addDouble(double value);
    void addString(const string& value);

    void addPara(const CommandPara& para);
    void update(const CommandPara& para);
    void clear();

    //  TODO GETLINES  考虑和getPreviewLines实现公用基础
    void getCompleteLines(list<Line>& lines) const;
    void getPreviewLines(list<Line>& lines) const;

    const Vector3f* GetCurLastValidPoint() const;

private:
    void getLines(const CommandPara& lastVert, list<Line>& lines) const;

};