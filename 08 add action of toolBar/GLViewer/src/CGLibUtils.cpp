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

void CGLibUtils::GetPolygonCenter(const list<Line>& lines, Vector3f& center)
{
    center.X = 0.f;
    center.Y = 0.f;
    center.Z = 0.f;
    for (auto& line : lines)
    {
        center = center + line.pt0;
    }
    center = center * (1.f / lines.size());
}

bool CGLibUtils::getClickHittedPlane(const Vector3f& rayOri, const Vector3f& rayEnd, const BodyInfo& bodyInfo, Vector3f& planePt, Vector3f& planeNormal)
{
    Vector3f rayDir = rayEnd - rayOri;
    rayDir.Normalize();

    if (!bodyInfo.current->lstFace.empty())
    {
        double targetT = -1.0;
        Face* targetFace = nullptr;
        for (auto& face : bodyInfo.current->lstFace)
        {
            //  TODO 考虑face的normal放到face中，避免重复计算
            Vector3f normalFace = CurveTool::GetNormal(face.outerLoop.lstLine);
            double t;
            if (CGUtils::IsRayHitFace(face, normalFace, rayOri, rayDir, t))
            {
                if (targetT == -1.0)
                {
                    targetT = t;
                    targetFace = &face;
                    planeNormal = normalFace;
                }

                targetT = targetT > t ? targetFace = &face, planeNormal = normalFace, t : targetT;
            }
        }

        if (!targetFace)
            return false;

        GetPolygonCenter(targetFace->outerLoop.lstLine, planePt);
    }
    else
    {
        double targetT = -1.0;
        const Vector3ui* targetFace = nullptr;
        auto mesh = bodyInfo.getCurrentMesh();
        for (size_t i = 0; i < mesh->GetFacetCount(); i++)
        {
            auto& facet = mesh->GetFacet(i);
            double t;
            if (CGUtils::IntersectTriangle(rayOri, rayDir, mesh->GetVertex(facet.X), mesh->GetVertex(facet.Y), mesh->GetVertex(facet.Z), &t))
            {
                if (targetT == -1.0)
                {
                    targetT = t;
                    targetFace = &facet;
                }

                targetT = targetT > t ? targetFace = &facet, t : targetT;
            }
        }

        if (!targetFace)
            return false;

        const Vector3f& pt0 = mesh->GetVertex(targetFace->X);
        const Vector3f& pt1 = mesh->GetVertex(targetFace->Y);
        const Vector3f& pt2 = mesh->GetVertex(targetFace->Z);

        planePt = (pt0 + pt1 + pt2) / 3.0;
        CGUtils::GetTriNormal(pt0, pt1, pt2, planeNormal);
    }

    return true;
}
