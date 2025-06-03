#pragma once
#include <vector>
#include <CGUtils/CGUtils.h>
#include "Mesh.h"
#include <CGLib/Vector3W.h>

using namespace std;
using namespace CGUTILS;

enum GLCurveType
{
    GLUnknown,
    GLLine,
    GLArc,
    GLCircle,
    GLEllipseArc,
    GLRectangle,
    //  TODO 样条曲线

    GLTemp,  //主要为了使用预览
    GLPoint,  //主要为了显示捕获点
};

enum GLNodeType
{
    GLUnKnown,
    GLCurveNode0,  //端点1
    GLCurveNode1,  //端点2
    GLCurveNodeM,  //中点
    GLCircleCenter,  //圆心
    GLOnCurve,  //其他线上点
    GLAxisNetNode,  //轴线交点
};

enum BodyInfoType
{
    B_Unknown,
    B_Construct,
    B_Boolean,
};

class MeshInfo
{
public:
    MeshInfo(shared_ptr<Mesh> body, const Vector3W& drawColor) : type(BodyInfoType::B_Construct), color(drawColor), selected(false)
    {
        data.push_back(body);
        current = data[0];
    }

    MeshInfo(const vector<shared_ptr<Mesh>>& bdBoolean, const Vector3W& drawColor) : type(BodyInfoType::B_Boolean), data(vector<shared_ptr<Mesh>>(4)), color(drawColor), selected(false)
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

    Vector3W color;

    bool selected;
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
        //  需要更新current指向
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

    //  对于布尔运算，data.size == 4。0为intersect，1为union，2为diffAB，3为diffBA
    vector<Body> data;
    vector<TriangleMesh> dataMesh;

    Body* current;
};

class ImageInfo
{
public:
    ImageInfo(const QString& file, shared_ptr<Mesh> ms, const Vector3f& nor, const QVector4D& colr, const Transform& trs) : imageFile(file), mesh(ms), normal(nor), color(colr), transform(trs) {}

    QString imageFile;
    shared_ptr<Mesh> mesh;
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

class ModelNode
{
public:
    ModelNode() : nodeType(GLNodeType::GLUnKnown), curveId(-1) {}
    ModelNode(GLNodeType nType, const Vector3f& pt, int curvId) : nodeType(nType), point(pt), curveId(curvId) {}

public:
    GLNodeType nodeType;
    Vector3f point;
    int curveId;
};

class CurveData
{
public:
    GLCurveType curveType;
    vector<Vector3f> vertex;
    string layerName;
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

class CommandPara
{
public:
    CommandPara() : vType(-1), pValue(), dValue(0.0), strValue("") {}
    CommandPara(const Vector3f& point) : vType(1), pValue(point) {}
    CommandPara(double value) : vType(2), dValue(value) {}
    CommandPara(const string& value) : vType(3), strValue(value) {}

public:
    void Init()
    {
        vType = -1;
    }

    void Init(const Vector3f& point)
    {
        vType = 1;
        pValue = point;
    }

    void Init(double value)
    {
        vType = 2;
        dValue = value;
    }

    void Init(const string& value)
    {
        vType = 3;
        strValue = value;
    }

    int vType;  //1:point  2:double  3:string
    Vector3f pValue;
    double dValue;
    string strValue;
};

class CGLibUtils
{
public:
    static void getContinusLines(const vector<Vector3f>& lstVert, list<Line>& lines);
    static void getPoints(const list<Line>& lines, list<Vector3f>& verts);

    static void GenerateCubeMesh(const vector<Vector3f>& sectionPts, const Vector3f& normal, float height, TriangleMesh& mesh);
    static void ConstructSphere(const Vector3f& center, double radius, Body& sphere);
    static void GetImageMesh(const Vector3f& normal, double length, double width, const Vector3f& insert, double rotation, float offset, TriangleMesh& mesh);

    static void getRectSection(const Vector3f& origin, const Vector3f& normal, float halfLen, float offset, Vector3f* pts);  //Vector3f pts[4]
    static void GetPolygonCenter(const list<Line>& lines, Vector3f& center);
    static bool getClickHittedPlane(const Vector3f& rayOri, const Vector3f& rayEnd, const BodyInfo& bodyInfo, Vector3f& planePt, Vector3f& planeNormal);

    static void sortLinesByLayer(const map<PointData_1e_3, vector<CurveData*>>& lines, const map<CurveData*, bool>& mapIgnoreCurveData, const map<CurveData*, bool>& mapTargetCurveData,
        map<string, map<PointData_1e_3, vector<CurveData*>>>& mapLayer2Lines);

    static void Transform2QMatrix4x4(const Transform& trs, QMatrix4x4& matRe);
    static shared_ptr<Mesh> ConvertMesh(QOpenGLFunctions_4_5_Core* glFunc, const TriangleMesh& mesh);

};
