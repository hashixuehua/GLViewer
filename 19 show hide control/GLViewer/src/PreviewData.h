#pragma once
#include "CGLibUtils.h"

class PreviewData
{
public:
    PreviewData();

    GLCurveType curveType;
    int drawType;  //���ַ�ʽdraw curve����[��ʼ�㡢��ֹ�㡢���ϵ�]��[Բ�ġ��뾶����ʼ���ȡ���ֹ����]���ɻ�Բ��
    Vector3f wkPlaneNormal;  //����ƽ�淨��
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

    //  TODO GETLINES  ���Ǻ�getPreviewLinesʵ�ֹ��û���
    void getCompleteLines(list<Line>& lines) const;
    void getPreviewLines(list<Line>& lines) const;

    const Vector3f* GetCurLastValidPoint() const;

private:
    void getLines(const CommandPara& lastVert, list<Line>& lines) const;

};