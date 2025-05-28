#pragma once
#include <string>
#include <map>
#include "LayerData.h"
#include "PreviewData.h"
using namespace std;

enum ViewerStatus
{
    Unknown,
    DrawingLines,
    DrawingArc,
    DrawingCircle,
    DrawingRectangle,

    DrawingImage,
    Copy,

    CSphere,
    SetWorkPlane,

};

class LayerCache
{
public:
    //  layer
    static string currentLayer;
    static map<string, LayerData> mapName2Layer;

    static string prefixOfLayerName;

private:
    static int layerCodeNum;

public:
    static const LayerData& getCurrentLayer();
    static const LayerData& getLayer(const string& layerName);
    static const LayerData& getDefaultLayer();

private:
    static LayerData& getLayer2(const string& layerName);

};

class ViewerSetting
{
public:
    //  适配不同客户端屏幕缩放比例
    static float devicePixelRatio;

    //  status
    static ViewerStatus viewerStatus;

    //  preview
    //Vector3f* previewNextPt;
    static PreviewData previewData;

    //  捕捉
    static bool snapMode;

    //  正交模式
    static bool orthogonalityMode;

    //  工作平面
    static Vector3f currentWorkPlPoint;
    static Vector3f currentWorkPlNormal;

public:
    static int IsStatusNeedDraw(ViewerStatus status);

private:
    static LayerData& getLayer2(const string& layerName);

};
