#include "CGLibUtils.h"

////////////  PeviewData  ////////////

PeviewData::PeviewData() :curveType(GLCurveType::GLUnknown), drawType(-1), wkPlaneNormal(Vector3f::BasicZ), previewNextPt(nullptr), complete(false) {}

void PeviewData::addPoint(const Vector3f& pt)
{
    addPara(pt);
}

void PeviewData::addDouble(double value)
{
    addPara(value);
}

void PeviewData::addString(const string& value)
{
    addPara(value);
}

void PeviewData::addPara(const CommandPara& para)
{
    if (complete)
        return;

    curData.push_back(para);
    if (curveType == GLCurveType::GLPoint && curData.size() == 1)
    {
        complete = true;
        return;
    }
    else if ((curveType == GLCurveType::GLLine || curveType == GLCurveType::GLTemp) && curData.size() == 2)
    {
        //  这里用的是多段线，只要有点就complete
        complete = true;
        return;
    }
    else if (curveType == GLCurveType::GLArc && curData.size() == 3)
    {
        complete = true;
        return;
    }
    else if (curveType == GLCurveType::GLCircle && curData.size() == 2)
    {
        complete = true;
        return;
    }
    else if (curveType == GLCurveType::GLRectangle && curData.size() >= 2)
    {
        complete = (curData.size() == 2 && curData[1].vType == 1) || (curData.size() == 3 && curData[1].vType == 2);
        return;
    }
}

void PeviewData::update(const CommandPara& para)
{
    complete = false;
    delete previewNextPt;
    previewNextPt = nullptr;

    if (curveType == GLCurveType::GLLine || curveType == GLCurveType::GLTemp)
    {
        curData.clear();
        curData.push_back(para);
    }
    else if (curveType == GLCurveType::GLArc
        || curveType == GLPoint
        || curveType == GLCurveType::GLCircle
        || curveType == GLCurveType::GLRectangle)
    {
        curData.clear();
    }
}

void PeviewData::clear()
{
    curveType = GLCurveType::GLUnknown;
    drawType = -1;
    wkPlaneNormal = Vector3f::BasicZ;
    curData.clear();
    delete previewNextPt;
    previewNextPt = nullptr;
    complete = false;
    //lstTempPt.clear();
}

//  TODO GETLINES  考虑和getPreviewLines实现公用基础

void PeviewData::getCompleteLines(list<Line>& lines) const
{
    getLines(curData.back(), lines);
}

void PeviewData::getPreviewLines(list<Line>& lines) const
{
    getLines(*previewNextPt, lines);
}

void constructCirclePoints(const vector<CommandPara>& source, int drawType, vector<Vector3f>& result)
{
    auto& lastVert = source.back();
    double dRadius = lastVert.vType == 1 ? dRadius = lastVert.pValue.Distance(source[0].pValue) : lastVert.dValue / 1000.0;

    Vector3f pt0 = source[0].pValue;
    Arc2d ptrArc(pt0, dRadius, 0.0, 2 * PI, false);

    for (size_t i = 0; i < 11; i++)
    {
        result.push_back(ptrArc.GetCurvePoint(0.1 * i));
    }
}

void constructArcPoints(const vector<CommandPara>& source, int drawType, vector<Vector3f>& result)
{
    auto& lastVert = source.back();
    Vector3f pt0 = source[0].pValue;
    Vector3f pt1 = source[1].pValue;

    Arc2d* ptrArc = nullptr;
    if (drawType == 0)  //start、end、ptOnCurve
    {
        ptrArc = new Arc2d(pt0, pt1, lastVert.pValue);
    }
    else if (drawType == 1)  //center、start、end
    {
        double angle = 0.0;
        if (lastVert.vType == 1)
        {
            Vector3f dirO2E = lastVert.pValue - pt0;
            Vector3f dirO2S = pt1 - pt0;
            angle = dirO2S.AngleOnPlaneTo(dirO2E, Vector3f::BasicZ);
        }
        else
            angle = lastVert.dValue / 180.0 * PI;

        ptrArc = new Arc2d(pt0, pt1, angle);
    }
    //  这里不做检查(ptrArc是否为空、drawType是否有效)，由调用者保证

    for (size_t i = 0; i < 11; i++)
    {
        result.push_back(ptrArc->GetCurvePoint(0.1 * i));
    }

    delete ptrArc;
}

void constructRectanglePoints(const vector<CommandPara>& source, int drawType, vector<Vector3f>& result)
{
    auto& lastVert = source.back();
    Vector3f pt0 = source[0].pValue;
    Vector3f pt1 = lastVert.pValue;

    //  calculate other 2 points
    Vector3f vecPt0To1 = pt1 - pt0;
    double dLen0To1 = vecPt0To1.Length();

    double ang = (source.size() == 2 || source[1].vType == 1) ? 0.0 : source[1].dValue;
    Transform trsRotate(Vector3f::BasicZ, ang / 180.0 * PI);
    Vector3f dirX, dirY;
    Transform::MultVector(trsRotate, Vector3f::BasicX, dirX);
    Transform::MultVector(trsRotate, Vector3f::BasicY, dirY);
    double dAngle = dirX.Angle(vecPt0To1);
    double dLength = dLen0To1 * cos(dAngle);
    Vector3f pt2 = pt0 + dLength * dirX;
    Vector3f pt3 = pt1 - dLength * dirX;

    result.push_back(pt0);
    result.push_back(pt2);
    result.push_back(pt1);
    result.push_back(pt3);
    result.push_back(*result.begin());
}

void constructLines(int drawType, const Vector3f& planeNormal, const vector<CommandPara>& curPara, const CommandPara& lastVert, const std::function<void(const vector<CommandPara>&, int, vector<Vector3f>&)>& func
    , list<Line>& result)
{
    //  TODO 后续考虑减少（source、lstPt）频繁的释放回收
    vector<CommandPara> source = curPara;
    source.push_back(lastVert);

    Transform* trs = nullptr;
    if (!planeNormal.IsEqual(Vector3f::BasicZ))
    {
        Vector3f localX, localY;
        CGUtils::getLocalXY(planeNormal, localX, localY);
        trs = new Transform(localX, localY, planeNormal, Vector3f::Zero);
    }

    if (trs != nullptr)
    {
        //  转到局部坐标系
        Transform trsW2L;
        Transform::GetInverse(*trs, trsW2L);

        for (auto& para : source)
        {
            if (para.vType == 1)
            {
                Transform::MultPoint(trsW2L, para.pValue, para.pValue);
            }
        }
    }

    vector<Vector3f> lstPt;
    func(source, drawType, lstPt);

    if (trs != nullptr)
    {
        for (auto& pt : lstPt)
        {
            Transform::MultPoint(*trs, pt, pt);
        }
    }

    //  TEST
    //if (!lstPt.begin()->IsEqual(curData[0]))
    //    throw exception("error.");
    //if (!lstPt.back().IsEqual(curData[1]))
    //    throw exception("error.");

    CGLibUtils::getContinusLines(lstPt, result);
    delete trs;
}

void PeviewData::getLines(const CommandPara& lastVert, list<Line>& lines) const
{
    if (curData.empty())
        return;

    //lstTempPt.clear();

    if (curveType == GLCurveType::GLPoint)
    {
        lines.push_back(Line{ curData[0].pValue, lastVert.pValue });
    }
    else if (curveType == GLCurveType::GLLine || curveType == GLCurveType::GLTemp)
    {
        lines.push_back(Line{ curData[0].pValue, lastVert.pValue });
    }
    else if (curveType == GLCurveType::GLArc)
    {
        if (curData.size() == 1)
        {
            lines.push_back(Line{ curData[0].pValue, lastVert.pValue });
            return;
        }

        if (curData.size() >= 2)
        {
            constructLines(drawType, wkPlaneNormal, curData, lastVert, constructArcPoints, lines);
        }
    }
    else if (curveType == GLCurveType::GLCircle)
    {
        if (curData.size() >= 1)
        {
            constructLines(drawType, wkPlaneNormal, curData, lastVert, constructCirclePoints, lines);
        }
    }
    else if (curveType == GLCurveType::GLRectangle)
    {
        if (curData.size() >= 1)
        {
            constructLines(drawType, wkPlaneNormal, curData, lastVert, constructRectanglePoints, lines);
        }
    }
}

const Vector3f* PeviewData::GetCurLastValidPoint() const
{
    for (auto itrItem = curData.rbegin(); itrItem != curData.rend(); ++itrItem)
    {
        if (itrItem->vType == 1)
            return &itrItem->pValue;
    }

    return nullptr;
}

////////////  PeviewData  ////////////

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
