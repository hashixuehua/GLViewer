#pragma once
#include <QVector4D>

class Setting
{
public:
    //  ���䲻ͬ�ͻ�����Ļ���ű���
    static float devicePixelRatio;

    //  MSAA sample size
    static int sampleSieOfMSAA;

    //  width of line
    static float edgeLineWidth;
    static float curveWidth;
    static float curveWidth4Selected;

    //  color
    static QVector4D edgeLineColor;
    static QVector4D wireframeColor;
    static QVector4D snapColor;
    static QVector4D rectBoxColor;

    //  width of outlinning
    static float outlinningWidth;

};