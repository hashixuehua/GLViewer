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
    static float edgeLineWidthArray[2];//0[edgeLine]; 1[wireframe]
    static float curveWidth;
    static float curveWidth4Selected;

    //  color
    static QVector4D edgeLineColor;
    static QVector4D edgeLineColorArray[4];//0[edgeLine]; 1[wireframe]; 2[snap]; 3[rectBox]

    //  width of outlinning
    static float outlinningWidth;

};