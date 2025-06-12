#pragma once
#include <vector>
#include <CGUtils/CGUtils.h>
#include "Mesh.h"

using namespace std;
using namespace CGUTILS;

enum BodyInfoType
{
    B_Unknown,
    B_Construct,
    B_Boolean,
};

class MeshInfo
{
public:
    MeshInfo(shared_ptr<Mesh> body) : type(BodyInfoType::B_Construct)
    {
        data.push_back(body);
        current = data[0];
    }

    MeshInfo(const vector<shared_ptr<Mesh>>& bdBoolean) : type(BodyInfoType::B_Boolean), data(vector<shared_ptr<Mesh>>(4))
    {
        if (bdBoolean.size() != 4)
            throw exception("error.");

        data = bdBoolean;
        current = data[0];
    }

    BodyInfoType type;

    //  对于布尔运算，data.size == 4。0为intersect，1为union，2为diffAB，3为diffBA
    vector<shared_ptr<Mesh>> data;

    shared_ptr<Mesh> current;
};

class WorkPlanePara
{
public:
    WorkPlanePara(const Vector3f& ori, const Vector3f& norm, const Vector3f& lX, const Vector3f& lY, float halfLen, int cntGridPerEdg, float off)
        : origin(ori), normal(norm), localX(lX), localY(lY), halfLength(halfLen), gridCntPerEdge(cntGridPerEdg), offset(off)
    {
    }

    Vector3f origin;
    Vector3f normal;
    Vector3f localX;
    Vector3f localY;
    float halfLength;
    int gridCntPerEdge;
    float offset;
};

class CGLibUtils
{
public:
    static void GenerateCubeMesh(const vector<Vector3f>& sectionPts, const Vector3f& normal, float height, TriangleMesh& mesh);
    static void getRectSection(const Vector3f& origin, const Vector3f& normal, float halfLen, float offset, Vector3f* pts);  //Vector3f pts[4]

};
