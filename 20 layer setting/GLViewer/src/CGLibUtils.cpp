#include "CGLibUtils.h"

void CGLibUtils::getContinusLines(const vector<Vector3f>& lstVert, list<Line>& lines)
{
    for (size_t i = 0; i < lstVert.size() - 1; i++)
    {
        lines.push_back(Line{ lstVert[i], lstVert[i + 1] });
    }
}

void CGLibUtils::getPoints(const list<Line>& lines, list<Vector3f>& verts)
{
    for (auto& line : lines)
    {
        verts.push_back(line.pt0);
        verts.push_back(line.pt1);

        for (size_t i = 1; i < 10; i++)
        {
            verts.push_back(i * 0.1 * line.pt0 + (1.0 - i * 0.1) * line.pt1);
        }
    }
}

void CGLibUtils::GenerateCubeMesh(const vector<Vector3f>& sectionPts, const Vector3f& normal, float height, TriangleMesh& mesh)
{
    Body cube;
    CGUtils::GenerateCube(sectionPts, normal, height, cube);

    CGUTILS::CGUtils::TessellateBody(cube, mesh);
}

void CGLibUtils::GetImageMesh(const Vector3f& normal, double length, double width, const Vector3f& insert, double rotation, float offset, TriangleMesh& mesh)
{
    //  local coordinate
    //Vector3f localX = normal.IsEqual(Vector3f::BasicX, 1e-1) ? Vector3f::BasicY : Vector3f::BasicX;
    //Vector3f localY = normal.CrossProduct(localX);
    //localY.Normalize();
    //localX = localY.CrossProduct(normal);
    Vector3f localX, localY;
    CGUtils::getLocalXY(normal, localX, localY);

    Vector3f pt0 = insert - length * localX - width * localY + offset * normal;
    Vector3f pt1 = insert + length * localX - width * localY + offset * normal;
    Vector3f pt2 = insert + length * localX + width * localY + offset * normal;
    Vector3f pt3 = insert - length * localX + width * localY + offset * normal;

    Vector3f pt4 = insert - length * localX - width * localY - offset * normal;
    Vector3f pt5 = insert + length * localX - width * localY - offset * normal;
    Vector3f pt6 = insert + length * localX + width * localY - offset * normal;
    Vector3f pt7 = insert - length * localX + width * localY - offset * normal;

    //Vector3f pt0(label.second.X - 100, label.second.Y - 100, label.second.Z + 0.01f);
    //Vector3f pt1(label.second.X + 100, label.second.Y - 100, label.second.Z + 0.01f);
    //Vector3f pt2(label.second.X + 100, label.second.Y + 100, label.second.Z + 0.01f);
    //Vector3f pt3(label.second.X - 100, label.second.Y + 100, label.second.Z + 0.01f);

    mesh.AddVertex(pt0);//正面
    mesh.AddVertex(pt1);
    mesh.AddVertex(pt2);
    mesh.AddVertex(pt3);
    mesh.AddVertex(pt7);//反面
    mesh.AddVertex(pt6);
    mesh.AddVertex(pt5);
    mesh.AddVertex(pt4);

    mesh.AddNormal(normal);
    mesh.AddNormal(normal);
    mesh.AddNormal(normal);
    mesh.AddNormal(normal);
    mesh.AddNormal(-normal);
    mesh.AddNormal(-normal);
    mesh.AddNormal(-normal);
    mesh.AddNormal(-normal);

    mesh.AddUV({ 0.0f, 1.0f });
    mesh.AddUV({ 1.0f, 1.0f });
    mesh.AddUV({ 1.0f, 0.0f });
    mesh.AddUV({ 0.0f, 0.0f });
    mesh.AddUV({ 0.0f, 0.0f });
    mesh.AddUV({ 1.0f, 0.0f });
    mesh.AddUV({ 1.0f, 1.0f });
    mesh.AddUV({ 0.0f, 1.0f });
    /*mesh.AddUV({ 1.0f, 0.0f });
    mesh.AddUV({ 0.0f, 0.0f });
    mesh.AddUV({ 0.0f, 1.0f });
    mesh.AddUV({ 1.0f, 1.0f });*/
    //mesh.AddUV({ 0.0f, 1.0f });
    //mesh.AddUV({ 1.0f, 1.0f });
    //mesh.AddUV({ 1.0f, 0.0f });
    //mesh.AddUV({ 0.0f, 0.0f });

    mesh.AddFacet({ 0u, 1u, 2u });
    mesh.AddFacet({ 0u, 2u, 3u });
    mesh.AddFacet({ 7u, 5u, 6u });
    mesh.AddFacet({ 7u, 4u, 5u });

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

void CGLibUtils::sortLinesByLayer(const map<PointData_1e_3, vector<CurveData*>>& lines, const map<CurveData*, bool>& mapIgnoreCurveData, const map<CurveData*, bool>& mapTargetCurveData,
    map<string, map<PointData_1e_3, vector<CurveData*>>>& mapLayer2Lines)
{
    for (auto& pair : lines)
    {
        for (auto& ls : pair.second)
        {
            if (mapIgnoreCurveData.find(ls) != mapIgnoreCurveData.end())
                continue;

            if (!mapTargetCurveData.empty() && mapTargetCurveData.find(ls) == mapTargetCurveData.end())
                continue;

            auto itrFind = mapLayer2Lines.find(ls->layerName);
            if (itrFind == mapLayer2Lines.end())
                mapLayer2Lines.insert(make_pair(ls->layerName, map<PointData_1e_3, vector<CurveData*>>())).first->second.insert(make_pair(pair.first, vector<CurveData*>{ ls }));
            else
            {
                auto itrFind2 = itrFind->second.find(pair.first);
                if (itrFind2 == itrFind->second.end())
                    itrFind->second.insert(make_pair(pair.first, vector<CurveData*>{ ls }));
                else
                    itrFind2->second.push_back(ls);
            }
        }
    }
}

void CGLibUtils::Transform2QMatrix4x4(const Transform& trs, QMatrix4x4& matRe)
{
    auto value = trs.GetValue();
    matRe.setColumn(0, QVector4D(value[0], value[1], value[2], 0.0));
    matRe.setColumn(1, QVector4D(value[3], value[4], value[5], 0.0));
    matRe.setColumn(2, QVector4D(value[6], value[7], value[8], 0.0));
    matRe.setColumn(3, QVector4D(value[9], value[10], value[11], 1.0));
}

shared_ptr<Mesh> CGLibUtils::ConvertMesh(QOpenGLFunctions_4_5_Core* glFunc, const TriangleMesh& mesh)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture*> textures;
    vector<unsigned int> edges;

    bool hasUV = mesh.GetVertexCount() == mesh.GetUVCount() && mesh.GetUVCount() != 0;

    for (size_t i = 0; i < mesh.GetVertexCount(); i++)
    {
        auto& ptSource = mesh.GetVertex(i);
        auto& ptNormal = mesh.GetNormal(i);

        Vertex vertex;
        vertex.Position.setX(ptSource.X / 1000.f);
        vertex.Position.setY(ptSource.Y / 1000.f);
        vertex.Position.setZ(ptSource.Z / 1000.f);

        vertex.Normal.setX(ptNormal.X);
        vertex.Normal.setY(ptNormal.Y);
        vertex.Normal.setZ(ptNormal.Z);

        if (hasUV)
        {
            auto& uv = mesh.GetUV(i);
            vertex.TexCoords.setX(uv.U);
            vertex.TexCoords.setY(uv.V);
        }
        else
        {
            vertex.TexCoords.setX(0.f);
            vertex.TexCoords.setY(0.f);
        }

        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh.GetFacetCount(); i++)
    {
        auto& face = mesh.GetFacet(i);
        indices.push_back(face.X);
        indices.push_back(face.Y);
        indices.push_back(face.Z);
    }

    for (size_t i = 0; i < mesh.GetEdgeCount(); i++)
    {
        auto& edge = mesh.GetEdge(i);
        edges.push_back(edge.U);
        edges.push_back(edge.V);
    }

    //result = Mesh(vertices, indices, textures, edges);
    return make_shared<Mesh>(glFunc, vertices, indices, textures, edges);
}

void CGLibUtils::ConstructSphere(const Vector3f& center, double radius, Body& sphere)
{
    //  北半球的纬度一共90°，10°一个圆
    Vector3f upOff;
    vector<vector<Vector3f>> arcsPts;
    for (size_t i = 0; i < 10; i++)
    {
        double angle = i * 10.0 / 180.0 * PI;
        double r = radius * cos(angle);
        upOff = radius * sin(angle) * Vector3f::BasicZ;
        Arc2d arc(center, r, 0.0, 2 * PI, false);

        vector<Vector3f> verts;
        for (size_t cntVert = 0; cntVert < 21; cntVert++)
        {
            auto vert = arc.GetCurvePoint(cntVert * 0.05);
            verts.push_back(vert + upOff);
        }
        arcsPts.push_back(verts);
    }

    for (size_t i = 0; i < arcsPts.size() - 1; i++)
    {
        auto& curPts = arcsPts[i];
        int next = i + 1;
        if (next == arcsPts.size() - 1)
        {
            //  next为点（极点）
            Vector3f vertTop = *arcsPts[i + 1].begin();
            for (size_t cntVert = 0; cntVert < curPts.size() - 1; cntVert++)
            {
                Vector3f vert0 = curPts[cntVert];
                Vector3f vert1 = curPts[cntVert + 1];

                Face face;
                face.outerLoop.lstLine.push_back({ vert0, vert1 });
                face.outerLoop.lstLine.push_back({ vert1, vertTop });
                face.outerLoop.lstLine.push_back({ vertTop, vert0 });
                sphere.lstFace.push_back(face);
            }
            continue;
        }

        //
        auto& nextPts = arcsPts[next];
        for (size_t cntVert = 0; cntVert < curPts.size() - 1; cntVert++)
        {
            Vector3f vert0 = curPts[cntVert];
            Vector3f vert1 = curPts[cntVert + 1];
            Vector3f vert2 = nextPts[cntVert + 1];
            Vector3f vert3 = nextPts[cntVert];

            Face face;
            face.outerLoop.lstLine.push_back({ vert0, vert1 });
            face.outerLoop.lstLine.push_back({ vert1, vert2 });
            face.outerLoop.lstLine.push_back({ vert2, vert3 });
            face.outerLoop.lstLine.push_back({ vert3, vert0 });
            sphere.lstFace.push_back(face);
        }
    }

    //  复制生成南半球
    //  对XOY平面镜像
    Transform trs(Vector3f::BasicX, Vector3f::BasicY, -Vector3f::BasicZ, 2.0 * center.Z * Vector3f::BasicZ);

    int size = sphere.lstFace.size();
    for (int cntItem = 0; cntItem < size; cntItem++)
    {
        Face face;
        CGUtils::GetInverseFace(sphere.lstFace[cntItem], face);
        //Face face = sphere.lstFace[cntItem];
        CGUtils::TransformFace(face, trs);
        sphere.lstFace.push_back(face);
    }

    //auto southPart = sphere;
    //CGUtils::TransformBody(southPart, trs);
    //
    //sphere.lstFace.insert(sphere.lstFace.end(), southPart.lstFace.begin(), southPart.lstFace.end());
}
