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
    //  call back
    static std::function<void(const LayerData&)> OnLayerRemoved;
    static std::function<void(const LayerData&)> OnLayerColorChanged;

private:
    //  layer
    static string currentLayer;
    static map<string, LayerData> mapName2Layer;

    static string prefixOfLayerName;
    static int layerCodeNum;

public:
    static const LayerData& getCurrentLayer();
    static void setCurrentLayer(const string& layerName);
    static const map<string, LayerData>& getAllLayer();

    static const LayerData& getLayer(const string& layerName);
    static const LayerData& getDefaultLayer();

    static string generateLayerName();

    static void addLayer(const LayerData& layer);
    static void removeLayer(const string& layerName);
    static void updateLayerColor(const string& layerName, const Vector3i& color);

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

    //  框选
    static bool isSelectMode;
    static Vector2f* rectSelectStart;
    static Vector2f* rectSelectEnd;

    //  copy
    static Vector3f* basePt4Copy;

    //  show or hide
    static bool showModel;
    static bool showLines;

public:
    static int IsStatusNeedDraw(ViewerStatus status);

    static bool IsRectSelectValid();
    static void UpdateRectSelect(int px, int py);
    static void ClearRectSelect(bool selectModel);

    static QVector3D GetCopyOffset();
    static bool IsCopyElementsPreviewValid();
    static void ClearCopyImData();

    static void SwitchModelShownType();
    static void SwitchLinesShownType();

private:

};
