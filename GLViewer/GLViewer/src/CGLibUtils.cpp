#include "CGLibUtils.h"

void CGLibUtils::GenerateCubeMesh(const vector<Vector3f>& sectionPts, const Vector3f& normal, float height, TriangleMesh& mesh)
{
    Body cube;
    CGUtils::GenerateCube(sectionPts, normal, height, cube);

    CGUTILS::CGUtils::TessellateBody(cube, mesh);
}

void CGLibUtils::getRectSection(const Vector3f& origin, const Vector3f& normal, float halfLen, float offset, Vector3f* pts)
{
    Vector3f localX, localY;
    CGUtils::getLocalXY(normal, localX, localY);

    Vector3f pt0 = origin - halfLen * localX - halfLen * localY + offset * normal;
    Vector3f pt1 = origin + halfLen * localX - halfLen * localY + offset * normal;
    Vector3f pt2 = origin + halfLen * localX + halfLen * localY + offset * normal;
    Vector3f pt3 = origin - halfLen * localX + halfLen * localY + offset * normal;

    //Vector3f pt0 = origin + Vector3f(-halfLen, -halfLen, offset);
    //Vector3f pt1 = origin + Vector3f(halfLen, -halfLen, offset);
    //Vector3f pt2 = origin + Vector3f(halfLen, halfLen, offset);
    //Vector3f pt3 = origin + Vector3f(-halfLen, halfLen, offset);

    pts[0] = pt0;
    pts[1] = pt1;
    pts[2] = pt2;
    pts[3] = pt3;
}
