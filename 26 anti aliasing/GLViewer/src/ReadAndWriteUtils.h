#pragma once
#define RAPIDJSON_HAS_STDSTRING 1

// RapidJSON uses constant if expressions to support multiple platforms
#pragma warning(push)
#pragma warning(disable:4127)
#include <rapidjson/schema.h>
#pragma warning(pop)

// This file is used as a catch-all for including rapidjson headers in the SDK. 
// If desired header not found below, please add it and include this file instead of the header directly
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <jsonUtils/JsonUtils.h>

#include <list>
#include "CGUtils/CGUtils.h"
#include "ViewerSetting.h"
#include "ViewerUtils.h"

using namespace std;
using namespace CGUTILS;
using namespace rapidjson;

class ReadAndWriteUtils
{
public:
	static void DeserializeFromJson(const wstring& filePath, list<pair<string, list<pair<Vector3f, list<Line>>>>>& lstLayerName2lstNormal2Lines);
	static void SerializeToJson(const map<PointData_1e_3, vector<CurveData*>>& lines, const wstring& filePath);


};