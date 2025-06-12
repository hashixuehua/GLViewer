#pragma once
#include <string>
#include "CGUtils/CGUtils.h"

using namespace std;
using namespace CGUTILS;

class ViewerUtils
{
public:
    static Vector3f normalizeToAxis(const Vector3f& dir, const Vector3f& localX, const Vector3f& localY, const Vector3f& localZ);

};

class ViewerCache
{
public:
    static string defaultWorkPlane;
    static string currentWorkPlane;

};
