#include "ViewerSetting.h"

float ViewerSetting::devicePixelRatio = 1.0;
ViewerStatus ViewerSetting::viewerStatus = ViewerStatus::Unknown;

PreviewData ViewerSetting::previewData;

//  捕捉
bool ViewerSetting::snapMode = true;

//  正交模式
bool ViewerSetting::orthogonalityMode = false;

//  工作平面
Vector3f ViewerSetting::currentWorkPlPoint = Vector3f::Zero;
Vector3f ViewerSetting::currentWorkPlNormal = Vector3f::BasicZ;

//  框选
bool ViewerSetting::isSelectMode = false;
Vector2f* ViewerSetting::rectSelectStart = nullptr;
Vector2f* ViewerSetting::rectSelectEnd = nullptr;

//  copy
Vector3f* ViewerSetting::basePt4Copy = nullptr;


int ViewerSetting::IsStatusNeedDraw(ViewerStatus status)
{
    if (status == ViewerStatus::DrawingLines ||
        status == ViewerStatus::Copy ||
        status == ViewerStatus::DrawingImage)
        return 1;//line
    else if (status == ViewerStatus::DrawingArc)
        return 2;//arc
    else if (status == ViewerStatus::CSphere)
        return 3;//point
    else if (status == ViewerStatus::SetWorkPlane)
        return 4;//point bfor set work plane
    else if (status == ViewerStatus::DrawingCircle)
        return 5;
    else if (status == ViewerStatus::DrawingRectangle)
        return 6;

    return -1;
}

bool ViewerSetting::IsRectSelectValid()
{
    return isSelectMode && rectSelectStart && rectSelectEnd;
}



void ViewerSetting::ClearRectSelect(bool selectModel)
{
    isSelectMode = selectModel;
    delete rectSelectStart;
    rectSelectStart = nullptr;
    delete rectSelectEnd;
    rectSelectEnd = nullptr;
}

void ViewerSetting::UpdateRectSelect(int px, int py)
{
    if (!ViewerSetting::rectSelectStart)
    {
        ViewerSetting::rectSelectStart = new Vector2f();
        ViewerSetting::rectSelectStart->SetValue(px, py);
    }
    else
    {
        if (!ViewerSetting::rectSelectEnd)
            ViewerSetting::rectSelectEnd = new Vector2f();
        ViewerSetting::rectSelectEnd->SetValue(px, py);
    }
}

QVector3D ViewerSetting::GetCopyOffset()
{
    Vector3f offset = *previewData.previewNextPt - *basePt4Copy;
    return QVector3D(offset.X, offset.Y, offset.Z);
}

bool ViewerSetting::IsCopyElementsPreviewValid()
{
    return basePt4Copy && previewData.previewNextPt;
}

void ViewerSetting::ClearCopyImData()
{
    delete basePt4Copy;
    basePt4Copy = nullptr;
}

////////////////////////

string LayerCache::currentLayer = "";
map<string, LayerData> LayerCache::mapName2Layer;
string LayerCache::prefixOfLayerName = "Layer";
int LayerCache::layerCodeNum = -1;

const LayerData& LayerCache::getCurrentLayer()
{
    return mapName2Layer.at(currentLayer);
}

const LayerData& LayerCache::getLayer(const string& layerName)
{
    return getLayer2(layerName);
}

LayerData& LayerCache::getLayer2(const string& layerName)
{
    auto itrFind = mapName2Layer.find(layerName);
    if (itrFind == mapName2Layer.end())
    {
        throw exception("error.");
        //return getCurrentLayer();
    }

    return itrFind->second;
}

const LayerData& LayerCache::getDefaultLayer()
{
    string defaultLN = prefixOfLayerName + std::to_string(0);
    auto itrFind = mapName2Layer.find(defaultLN);
    if (itrFind == mapName2Layer.end())
    {
        mapName2Layer.insert(make_pair(defaultLN, LayerData(defaultLN, Vector3i(255, 0, 0), "deault")));
        layerCodeNum = 0;
        currentLayer = defaultLN;
    }

    return mapName2Layer.at(defaultLN);
}

//////


