#pragma once
#include <vector>
#include <CGUtils/CGUtils.h>
#include "Mesh.h"
#include <CGLib/Vector3W.h>

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
    MeshInfo(shared_ptr<Mesh> body, const Vector3W& drawColor) : type(BodyInfoType::B_Construct), color(drawColor)
    {
        data.push_back(body);
        current = data[0];
    }

    MeshInfo(const vector<shared_ptr<Mesh>>& bdBoolean, const Vector3W& drawColor) : type(BodyInfoType::B_Boolean), data(vector<shared_ptr<Mesh>>(4)), color(drawColor)
    {
        if (bdBoolean.size() != 4)
            throw exception("error.");

        data = bdBoolean;
        current = data[0];
    }

    BodyInfoType type;

    //  ���ڲ������㣬data.size == 4��0Ϊintersect��1Ϊunion��2ΪdiffAB��3ΪdiffBA
    vector<shared_ptr<Mesh>> data;

    shared_ptr<Mesh> current;

    Vector3W color;
};

class BodyInfo
{
public:
    BodyInfo(const Body& body, const TriangleMesh& triMesh) : type(BodyInfoType::B_Construct)
    {
        data.push_back(body);
        dataMesh.push_back(triMesh);
        current = &data[0];
    }

    BodyInfo(const vector<Body>& bdBoolean, const vector<TriangleMesh>& meshBoolean) : type(BodyInfoType::B_Boolean), data(vector<Body>(4))
    {
        if (bdBoolean.size() != 4)
            throw exception("error.");

        data = bdBoolean;
        dataMesh = meshBoolean;
        current = &data[0];
    }

    BodyInfo(const BodyInfo& other) : type(other.type), data(other.data), dataMesh(other.dataMesh)
    {
        //  ��Ҫ����currentָ��
        int curIndex = getCurrentIndex(other);
        current = &data[curIndex];
    }

public:
    int getCurrentIndex(const BodyInfo& bdInfo) const
    {
        for (size_t i = 0; i < bdInfo.data.size(); i++)
        {
            if (&(bdInfo.data[i]) == bdInfo.current)
            {
                return i;
            }
        }

        throw exception("getCurrentIndex.");
        //return -1;
    }

    const TriangleMesh* getCurrentMesh() const
    {
        int curIndex = getCurrentIndex(*this);
        return &dataMesh[curIndex];
    }

    BodyInfoType type;

    //  ���ڲ������㣬data.size == 4��0Ϊintersect��1Ϊunion��2ΪdiffAB��3ΪdiffBA
    vector<Body> data;
    vector<TriangleMesh> dataMesh;

    Body* current;
};

class ImageInfo
{
public:
    ImageInfo(const QString& file, Mesh* ms, const Vector3f& nor, const QVector4D& colr, const Transform& trs) : imageFile(file), mesh(ms), normal(nor), color(colr), transform(trs) {}

    QString imageFile;
    Mesh* mesh;
    Vector3f normal;
    QVector4D color;

    Transform transform;
};

class ImageTexture
{
public:
    ImageTexture() : texture(0), originWdith(0), originHeight(0) {}
    ImageTexture(QOpenGLTexture* ptrTexture, double oWidth, double oHeight) : texture(ptrTexture), originWdith(oWidth), originHeight(oHeight) {}

    QOpenGLTexture* texture;

    int originWdith;
    int originHeight;
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
    static void GetPolygonCenter(const list<Line>& lines, Vector3f& center);
    static bool getClickHittedPlane(const Vector3f& rayOri, const Vector3f& rayEnd, const BodyInfo& bodyInfo, Vector3f& planePt, Vector3f& planeNormal);

};
