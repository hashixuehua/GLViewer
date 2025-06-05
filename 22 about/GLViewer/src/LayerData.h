#pragma once

#include <string>
#include "CGLib/Vector3.h"

using namespace std;
using namespace CGUTILS;

class LayerData
{
public:
    LayerData(const string& strName, const Vector3i& colorValue, const string& des)
        : name(strName), color(colorValue), description(des) {
    }

    string name;
    Vector3i color;
    string description;

};
