#include "ReadAndWriteUtils.h"

void getVertexes(const rapidjson::Value& value, vector<Vector3f>& lstVertex)
{
    auto dataArray = value.GetArray();
    for (size_t i = 0; i < dataArray.Size() / 3; i++)
    {
        float x = dataArray[i * 3].GetFloat();
        float y = dataArray[i * 3 + 1].GetFloat();
        float z = dataArray[i * 3 + 2].GetFloat();
        lstVertex[i] = Vector3f{ x, y, z };
    }
}

void getIndices(const rapidjson::Value& value, list<Vector2ui>& lstIndice)
{
    auto dataArray = value.GetArray();
    for (size_t i = 0; i < dataArray.Size() / 2; i++)
    {
        unsigned int x = dataArray[i * 2].GetUint();
        unsigned int y = dataArray[i * 2 + 1].GetUint();
        lstIndice.push_back(Vector2ui{ x, y });
    }
}

template <typename T>
void getVector3(const rapidjson::Value& value, Vector3<T>& normal)
{
    auto dataArray = value.GetArray();
    if (dataArray.Size() != 3)
        throw exception("format error.");

    if (std::is_same<T, float>::value)
    {
        float x = dataArray[0].GetFloat();
        float y = dataArray[1].GetFloat();
        float z = dataArray[2].GetFloat();
        normal.SetValue(x, y, z);
    }
    else if (std::is_same<T, int>::value)
    {
        int x = dataArray[0].GetInt();
        int y = dataArray[1].GetInt();
        int z = dataArray[2].GetInt();
        normal.SetValue(x, y, z);
    }
}

void getLines(const vector<Vector3f>& lstVert, const list<Vector2ui>& lstIndice, list<Line>& lines)
{
    for (auto& pair : lstIndice)
    {
        lines.push_back(Line{ lstVert[pair.U], lstVert[pair.V] });
    }
}

template <typename T>
void getVector3(const string& name, const Vector3<T>& normal, Document::AllocatorType& allocator, rapidjson::Value& value)
{
    rapidjson::Value rNormal(rapidjson::kArrayType);
    rNormal.PushBack(normal.X, allocator);
    rNormal.PushBack(normal.Y, allocator);
    rNormal.PushBack(normal.Z, allocator);

    rapidjson::Value vName;
    value.AddMember(vName.SetString(name.c_str(), allocator), rNormal, allocator);
}

int getLayerCodeNum(const string& layerName)
{
    size_t pos = layerName.find_last_of(LayerCache::getPrefixOfLayerName());
    if (pos == string::npos || pos == layerName.size() - 1)
        return -1;

    string subStr = layerName.substr(pos + 1);
    if (ViewerUtils::isNumber(subStr))
        return std::stoi(subStr);

    return -1;
}

void getLayers(const rapidjson::Value& value, map<string, LayerData>& mapName2Layer, string& activatedLayer, int& layerCodeNum)
{
    layerCodeNum = -1;
    auto dataArray = value.GetArray();
    for (size_t i = 0; i < dataArray.Size(); i++)
    {
        auto& data = dataArray[i];
        string nameData = data["name"].GetString();
        Vector3i color;
        getVector3(data["color"], color);
        string descriptionData = data["description"].GetString();
        if (data["activated"].GetBool())
            activatedLayer = nameData;

        LayerData layerData(nameData, color, descriptionData);
        mapName2Layer.insert(make_pair(layerData.name, layerData));

        int code = getLayerCodeNum(nameData);
        if (code > layerCodeNum)
            layerCodeNum = code;
    }

    if (mapName2Layer.size() > (layerCodeNum + 1))
        layerCodeNum = mapName2Layer.size() - 1;
}

void ReadLinesData(const rapidjson::Value& dataArray, list<pair<Vector3f, list<Line>>>& lstNormal2Lines)
{
    for (size_t i = 0; i < dataArray.Size(); i++)
    {
        auto& data = dataArray[i];
        auto& vertexData = data["vertex"];
        auto& indiceData = data["indice"];
        auto& normalData = data["normal"];

        vector<Vector3f> lstVert(vertexData.Size() / 3);
        getVertexes(vertexData, lstVert);

        list<Vector2ui> lstIndice;
        getIndices(indiceData, lstIndice);

        Vector3f normal;
        getVector3(normalData, normal);

        lstNormal2Lines.push_back(make_pair(normal, list<Line>()));
        getLines(lstVert, lstIndice, lstNormal2Lines.back().second);
    }
}

void getLines(const vector<CurveData*>& datas, Document::AllocatorType& allocator, rapidjson::Value& value)
{
    //  这里仅考虑线内部的共点
    rapidjson::Value rVert(rapidjson::kArrayType);
    rapidjson::Value rIndi(rapidjson::kArrayType);

    for (auto& data : datas)
    {
        for (size_t cntVert = 0; cntVert < data->vertex.size(); cntVert++)
        {
            auto vert = data->vertex[cntVert] * 1000.0;
            rVert.PushBack(vert.X, allocator);
            rVert.PushBack(vert.Y, allocator);
            rVert.PushBack(vert.Z, allocator);

            if (cntVert != data->vertex.size() - 1)
            {
                rIndi.PushBack(rVert.Size() / 3 - 1, allocator);
                rIndi.PushBack(rVert.Size() / 3, allocator);
            }
        }
    }

    value.AddMember("vertex", rVert, allocator);
    value.AddMember("indice", rIndi, allocator);
}

void FillLayerData(const map<string, LayerData>& mapName2Layer, const string& activeLayer, rapidjson::Value& layerValue, Document::AllocatorType& allocator)
{
    for (auto& layer : mapName2Layer)
    {
        rapidjson::Value item(rapidjson::kObjectType);

        rapidjson::Value vName, vDes;
        item.AddMember("name", vName.SetString(layer.second.name.c_str(), allocator), allocator);
        getVector3("color", layer.second.color, allocator, item);
        item.AddMember("description", vName.SetString(layer.second.description.c_str(), allocator), allocator);
        item.AddMember("activated", layer.second.name == activeLayer, allocator);

        layerValue.PushBack(item, allocator);
    }
}

void FillLinesItemData(const map<PointData_1e_3, vector<CurveData*>>& lines, rapidjson::Value& lineItemValue, Document::AllocatorType& allocator)
{
    for (auto& pair : lines)
    {
        rapidjson::Value item(rapidjson::kObjectType);

        getVector3("normal", pair.first.point, allocator, item);
        getLines(pair.second, allocator, item);

        lineItemValue.PushBack(item, allocator);
    }
}

void FillLineData(const map<string, map<PointData_1e_3, vector<CurveData*>>>& mapLayer2Lines, rapidjson::Value& lineValue, Document::AllocatorType& allocator)
{
    for (auto& pair : mapLayer2Lines)
    {
        rapidjson::Value item(rapidjson::kObjectType);

        //  layer
        rapidjson::Value vLayerName;
        item.AddMember("layer", vLayerName.SetString(pair.first.c_str(), allocator), allocator);

        //  lines
        rapidjson::Value linesValue(rapidjson::kArrayType);
        FillLinesItemData(pair.second, linesValue, allocator);

        item.AddMember("lines", linesValue, allocator);

        lineValue.PushBack(item, allocator);
    }
}

void ReadAndWriteUtils::DeserializeFromJson(const wstring& filePath, list<pair<string, list<pair<Vector3f, list<Line>>>>>& lstLayerName2lstNormal2Lines)
{
    Document docJson(rapidjson::kArrayType);
    ReadFileToJson(filePath, docJson);

    if (docJson.IsArray())
    {
        //  first version
        auto& defaultLayer = LayerCache::getDefaultLayer();

        lstLayerName2lstNormal2Lines.push_back(make_pair(defaultLayer.name, list<pair<Vector3f, list<Line>>>()));
        auto dataArray = docJson.GetArray();
        ReadLinesData(dataArray, lstLayerName2lstNormal2Lines.back().second);

        return;
    }

    if (docJson.IsObject() && docJson.HasMember("version"))
    {
        //  read version number
        int layerCodeNum = -1;
        map<string, LayerData> mapName2Layer;
        string activeLayer = "";
        auto& version = docJson["version"];
        if (strcmp(version.GetString(), "1.2") == 0)
        {
            //  layer
            auto& layerData = docJson["layer"];
            getLayers(layerData, mapName2Layer, activeLayer, layerCodeNum);

            LayerCache::InitLayerData(mapName2Layer, activeLayer, layerCodeNum);

            //  data
            auto& lineData = docJson["data"];
            for (size_t i = 0; i < lineData.Size(); i++)
            {
                auto& data = lineData[i];
                string lyName = data["layer"].GetString();

                lstLayerName2lstNormal2Lines.push_back(make_pair(lyName, list<pair<Vector3f, list<Line>>>()));
                ReadLinesData(data["lines"], lstLayerName2lstNormal2Lines.back().second);
            }
        }
    }
}

void ReadAndWriteUtils::SerializeToJson(const map<PointData_1e_3, vector<CurveData*>>& lines, const wstring& filePath)
{
    string currentVersion = "1.2";

    Document docJson(rapidjson::kObjectType);
    Document::AllocatorType& allocator = docJson.GetAllocator();

    if (currentVersion == "1.2")
    {
        map<string, map<PointData_1e_3, vector<CurveData*>>> mapLayer2Lines;
        map<CurveData*, bool> emptyMap;
        CGLibUtils::sortLinesByLayer(lines, emptyMap, emptyMap, mapLayer2Lines);

        //  version
        docJson.AddMember("version", "1.2", allocator);

        //  layer
        rapidjson::Value layerValue(rapidjson::kArrayType);
        FillLayerData(LayerCache::getAllLayer(), LayerCache::getCurrentLayer().name, layerValue, allocator);
        docJson.AddMember("layer", layerValue, allocator);

        //  data
        rapidjson::Value dataValue(rapidjson::kArrayType);
        FillLineData(mapLayer2Lines, dataValue, allocator);
        docJson.AddMember("data", dataValue, allocator);
    }
    else if (currentVersion == "1.0")
    {
        FillLinesItemData(lines, docJson, allocator);
    }

    SaveJsonToFile(docJson, filePath);
}

bool ReadAndWriteUtils::ReadFileToJson(const std::wstring& filePath, rapidjson::Document& document)
{
    std::ifstream in;
#if defined(WIN32)||defined(_WIN32)
    in.open(filePath, std::ifstream::in);
#else 
    in.open(UnicodeToUtf8(filePath.c_str()), std::ifstream::in);
#endif
    if (!in.is_open())
        return false;

    rapidjson::IStreamWrapper inWrapper(in);
    document.ParseStream<rapidjson::IStreamWrapper>(inWrapper);
    in.close();
    return true;
}

bool ReadAndWriteUtils::SaveJsonToFile(const rapidjson::Document& document, const std::wstring& filePath)
{
    std::ofstream out;
#if defined(WIN32)||defined(_WIN32)
    out.open(filePath, std::ifstream::out);
#else 
    out.open(UnicodeToUtf8(filePath.c_str()), std::ifstream::in);
#endif
    if (out.fail())
        return false;

    rapidjson::OStreamWrapper outWrapper(out);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(outWrapper);
    document.Accept(writer);
    out.close();
    return true;
}

