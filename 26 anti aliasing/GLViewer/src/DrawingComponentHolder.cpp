#include "DrawingComponentHolder.h"

DrawingComponentHolder::DrawingComponentHolder(QOpenGLFunctions_4_5_Core* glFunc) :m_glFunc(glFunc)
{
}

DrawingComponentHolder::~DrawingComponentHolder()
{
    for (auto& mesh : meshes)
    {
        delete mesh;
        mesh = nullptr;
    }
    for (auto& body : bodys)
    {
        delete body;
        body = nullptr;
    }
}

void DrawingComponentHolder::setup()
{
    Body hColumn;
    CGUtils::GenerateHColumn(300.f, 100.f, 120.f, 10.f, 12.f, 12.f, 800.f, Vector3f(1000.f, 1000.f, 0.0), hColumn);
    Body circleColumn;
    CGUtils::GenerateCircleColumn(80.f, 800.f, Vector3f(2000.f, 1000.f, 0.f), circleColumn);

    Body lofting;
    GenerateLofting(lofting);  //放样

    Body body1, body2;
    GenerateBody4(body1);
    GenerateBody5(body2);

    vector<Face> reTemp;
    CGUTILS::CGUtils::BodyDifference(body1, body2, reTemp);

    Body body3;
    GenerateBody1(body3);
    Body body4{ reTemp };

    //  倒角
    CGUTILS::CGUtils::Chamfer(body3, body3.lstFace[0].outerLoop.lstLine.back(), 20, true);

    //  布尔运算
    vector<Face> interFaces, unionFaces, body1Diff2Faces, body2Diff1Faces;
    CGUTILS::CGUtils::BodyIntersect(body3, body4, interFaces);
    CGUTILS::CGUtils::BodyUnion(body3, body4, unionFaces);
    CGUTILS::CGUtils::BodyDifference(body3, body4, body1Diff2Faces);
    CGUTILS::CGUtils::BodyDifference(body4, body3, body2Diff1Faces);

    //  切割带洞口的立方体
    Face cutFace;
    GetCutFace(Vector3f(0.0f, 0.0f, 0.0f), cutFace);
    list<Body> keepParts, cuttedParts;
    CGUTILS::CGUtils::Cut(body4, cutFace, keepParts, cuttedParts);

    //  切割H型柱
    Face cutFaceH;
    GetCutFace(Vector3f(1000.0f, 1000.0f, 0.0f), cutFaceH);
    list<Body> keepPartsH, cuttedPartsH;
    CGUTILS::CGUtils::Cut(hColumn, cutFaceH, keepPartsH, cuttedPartsH);

    //  color
    Vector3W color1(1.0f, 0.5f, 0.31f, 0.8f);
    Vector3W color2(0.1f, 0.5f, 1.0f, 0.8f);
    Vector3W color3(0.1f, 1.0f, 0.1f, 0.8f);
    Vector3W color4(1.0f, 0.0f, 1.0f, 0.8f);
    Vector3W color5(0.0f, 1.0f, 1.0f, 0.8f);

    //  trans
    auto trs = Transform::CreateIdentity();

    Body bodyInter{ interFaces };
    Body bodyUnion{ unionFaces };
    Body body1Diff2{ body1Diff2Faces };
    Body body2Diff1{ body2Diff1Faces };

    vector<Body*> lstBody{ &body4, &bodyInter, &bodyUnion, &body1Diff2, &body2Diff1 };

    for (size_t i = 0; i < lstBody.size(); i++)
    {
        trs->SetTranslate(Vector3f(1000.f * (i /*+ 1*/), 0.f, 0.f));
        CGUTILS::CGUtils::TransformBody(*lstBody[i], *trs);
    }

    //  偏移切割1
    trs->SetTranslate(Vector3f(100.0f, 1000.0f, 0.0f));
    for (auto& item : keepParts)
    {
        CGUTILS::CGUtils::TransformBody(item, *trs);
    }
    trs->SetTranslate(Vector3f(-100.0f, 1000.0f, 0.0f));
    for (auto& item : cuttedParts)
    {
        CGUTILS::CGUtils::TransformBody(item, *trs);
    }

    //  偏移切割2
    trs->SetTranslate(Vector3f(100.0f, 1000.0f, 0.0f));
    for (auto& item : keepPartsH)
    {
        CGUTILS::CGUtils::TransformBody(item, *trs);
    }
    trs->SetTranslate(Vector3f(-100.0f, 1000.0f, 0.0f));
    for (auto& item : cuttedPartsH)
    {
        CGUTILS::CGUtils::TransformBody(item, *trs);
    }

    Transform::Destroy(trs);

    TriangleMesh meshBody1;
    TriangleMesh meshBody2;
    CGUTILS::CGUtils::TessellateBody(body3, meshBody1);
    CGUTILS::CGUtils::TessellateBody(body4, meshBody2);

    TriangleMesh meshInter, meshUnion, mesh1Diff2, mesh2Diff1;
    CGUTILS::CGUtils::TessellateBody(bodyInter, meshInter);
    CGUTILS::CGUtils::TessellateBody(bodyUnion, meshUnion);
    CGUTILS::CGUtils::TessellateBody(body1Diff2, mesh1Diff2);
    CGUTILS::CGUtils::TessellateBody(body2Diff1, mesh2Diff1);

    TriangleMesh meshHColumn;
    CGUTILS::CGUtils::TessellateBody(hColumn, meshHColumn);
    TriangleMesh meshCircleColumn;
    CGUTILS::CGUtils::TessellateBody(circleColumn, meshCircleColumn);
    TriangleMesh meshLofting;
    CGUTILS::CGUtils::TessellateBody(lofting, meshLofting);
    TriangleMesh meshA, meshB, meshCSGOctTree_Inter, meshCSGOctTree_ADiffB, meshCSGOctTree_BDiffA, meshCSGOctTree_Union;
    BooleanOperateUsingCSGOctTree(meshA, meshB, meshCSGOctTree_Inter, meshCSGOctTree_ADiffB, meshCSGOctTree_BDiffA, meshCSGOctTree_Union);
    //TriangleMesh boxesIn, boxesOut, boxesCut;
    //BooleanOperateUsingCSGOctTree_TEST(boxesIn, boxesOut, boxesCut);

    //  三角网格布尔运算
    TriangleMesh cubeMesh;
    vector<Vector3f> sectionCube{ Vector3f{ 0.0, 3000.0, 200.0 }, Vector3f{ 1000.0, 3000.0, 200.0 }, Vector3f{ 1000.0, 6000.0, 200.0 }, Vector3f{ 0.0, 6000.0, 200.0 } };
    CGLibUtils::GenerateCubeMesh(sectionCube, Vector3f::BasicZ, 500.0, cubeMesh);

    TriangleMesh hColumnMesh;
    CGUtils::GenerateHColumnMesh(300.f, 100.f, 120.f, 10.f, 12.f, 12.f, 800.f, Vector3f(100.f, 4500.f, 0.0), hColumnMesh);

    TriangleMesh interMesh;
    CGUTILS::CGUtils::BodyIntersect(cubeMesh, hColumnMesh, interMesh, false);

    TriangleMesh unionMesh;
    CGUTILS::CGUtils::BodyUnion(cubeMesh, hColumnMesh, unionMesh, false);

    TriangleMesh diffABMesh;
    CGUTILS::CGUtils::BodyDifference(cubeMesh, hColumnMesh, diffABMesh, false);

    TriangleMesh diffBAMesh;
    CGUTILS::CGUtils::BodyDifference(hColumnMesh, cubeMesh, diffBAMesh, false);

    trs = Transform::CreateIdentity();
    trs->SetTranslate(Vector3f(2000.0f, 0.0f, 0.0f));
    interMesh.Transform(*trs);
    trs->SetTranslate(Vector3f(4000.0f, 0.0f, 0.0f));
    unionMesh.Transform(*trs);
    trs->SetTranslate(Vector3f(6000.0f, 0.0f, 0.0f));
    diffABMesh.Transform(*trs);
    trs->SetTranslate(Vector3f(8000.0f, 0.0f, 0.0f));
    diffBAMesh.Transform(*trs);
    Transform::Destroy(trs);

    //  sphere
    Body sphere;
    CGLibUtils::ConstructSphere(Vector3f(-2000.0, -1000.0, 1000.0), 300.0, sphere);
    TriangleMesh sphereMesh;
    CGUtils::TessellateBody(sphere, sphereMesh);

    //  add to scene
    bodys.push_back(new BodyInfo(body3, meshBody1));
    bodys.push_back(new BodyInfo(body4, meshBody2));
    bodys.push_back(new BodyInfo(bodyInter, meshInter));
    bodys.push_back(new BodyInfo(bodyUnion, meshUnion));
    bodys.push_back(new BodyInfo(body1Diff2, mesh1Diff2));
    bodys.push_back(new BodyInfo(body2Diff1, mesh2Diff1));
    bodys.push_back(new BodyInfo(hColumn, meshHColumn));
    bodys.push_back(new BodyInfo(circleColumn, meshCircleColumn));
    bodys.push_back(new BodyInfo(lofting, meshLofting));

    //  TODO 每个mesh对应一个Body，暂时让部分mesh对应空的body
    bodys.push_back(new BodyInfo(Body(), meshCSGOctTree_Inter));
    bodys.push_back(new BodyInfo(Body(), cubeMesh));
    bodys.push_back(new BodyInfo(Body(), hColumnMesh));
    bodys.push_back(new BodyInfo(Body(), interMesh));
    bodys.push_back(new BodyInfo(Body(), unionMesh));
    bodys.push_back(new BodyInfo(Body(), diffABMesh));
    bodys.push_back(new BodyInfo(Body(), diffBAMesh));
    bodys.push_back(new BodyInfo(sphere, sphereMesh));

    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshBody1), color1));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshBody2), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshInter), color3));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshUnion), color3));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh1Diff2), color1));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh2Diff1), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshHColumn), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshCircleColumn), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshLofting), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshCSGOctTree_Inter), color1));
    //meshes.push_back(ConvertMesh(boxesIn));
    //meshes.push_back(ConvertMesh(boxesOut));
    //meshes.push_back(ConvertMesh(boxesCut));

    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, cubeMesh), color1));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, hColumnMesh), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, interMesh), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, unionMesh), color1));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, diffABMesh), color1));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, diffBAMesh), color2));
    meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, sphereMesh), color2));

    //  cut info
    for (auto& item : keepParts)
    {
        TriangleMesh mesh;
        CGUTILS::CGUtils::TessellateBody(item, mesh);
        bodys.push_back(new BodyInfo(item, mesh));
        meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color4));
    }
    for (auto& item : cuttedParts)
    {
        TriangleMesh mesh;
        CGUTILS::CGUtils::TessellateBody(item, mesh);
        bodys.push_back(new BodyInfo(item, mesh));
        meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color4));
    }

    //
    for (auto& item : keepPartsH)
    {
        TriangleMesh mesh;
        CGUTILS::CGUtils::TessellateBody(item, mesh);
        bodys.push_back(new BodyInfo(item, mesh));
        meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color5));
    }
    for (auto& item : cuttedPartsH)
    {
        TriangleMesh mesh;
        CGUTILS::CGUtils::TessellateBody(item, mesh);
        bodys.push_back(new BodyInfo(item, mesh));
        meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color5));
    }
}

void DrawingComponentHolder::draw(QOpenGLShaderProgram& shader)
{
    //  model
    m_glFunc->glLineWidth(2.0f);
    if (ViewerSetting::showModel)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            auto& mesh = meshes[i];
            shader.setUniformValue("objectColor", mesh->color.GetWX(), mesh->color.GetWY(), mesh->color.GetWZ(), mesh->color.GetW());
            meshes[i]->current->Draw(shader, ViewerSetting::wireframeMode);
        }
    }
}

void DrawingComponentHolder::drawSelected(QOpenGLShaderProgram& shader, bool drawScaled /*= false*/)
{
    //  model
    m_glFunc->glLineWidth(2.0f);
    if (ViewerSetting::showModel)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            auto& mesh = meshes[i];
            if (!mesh->selected)
                continue;

            if (drawScaled)
            {
                //  first, translate to center; second, scale; last, translate to current pos.
                Vector3f boxMin, boxMax;
                bodys[i]->getCurrentMesh()->GetBoundingBox(boxMin, boxMax);
                Vector3f vecCross = boxMax - boxMin;

                //  TODO 后续进行统一参数设置管理
                float xScale = std::min(1.f + ViewerSetting::outlinningWidth / vecCross.X, 2.f);
                float yScale = std::min(1.f + ViewerSetting::outlinningWidth / vecCross.Y, 2.f);
                float zScale = std::min(1.f + ViewerSetting::outlinningWidth / vecCross.Z, 2.f);
                Vector3f center = 0.5 * (boxMin + boxMax) / 1000.0;

                QMatrix4x4 mat;
                mat.translate(center.X, center.Y, center.Z);
                mat.scale(xScale, yScale, zScale);
                mat.translate(-center.X, -center.Y, -center.Z);

                mat = ViewerSetting::matrixYup2Zup * mat;
                shader.setUniformValue("model", mat);
            }

            shader.setUniformValue("objectColor", mesh->color.GetWX(), mesh->color.GetWY(), mesh->color.GetWZ(), mesh->color.GetW());
            meshes[i]->current->Draw(shader, ViewerSetting::wireframeMode);
        }
    }
}

void DrawingComponentHolder::drawSelectionInfo(QOpenGLShaderProgram& shader)
{
    if (ViewerSetting::showModel)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            //Vector3f color = colors.empty() ? Vector3f(1.0f, 0.5f, 0.31f) : colors[i];
            //shader.setUniformValue("objectColor", color.X, color.Y, color.Z);
            shader.setUniformValue("elementId", i);
            meshes[i]->current->Draw(shader);
        }
    }
}

void DrawingComponentHolder::drawCopyElementsPreview(QOpenGLShaderProgram& shader)
{
    //  实例化的思路，复用之前的数据，设置对应的transform
    //  model elements
    QVector4D colorGray(0.75f, 0.75f, 0.75f, 1.0f);
    //QVector4D colorEdge(0.0f, 1.0f, 1.0f, 0.8f);

    if (ViewerSetting::showModel)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            if (!meshes[i]->selected)
                continue;

            shader.setUniformValue("objectColor", colorGray);
            meshes[i]->current->Draw(shader);
        }
    }
}

void DrawingComponentHolder::setSelected(int elementId, bool addMode)
{
    if (!addMode)
        clearSelected();

    meshes[elementId]->selected = true;
}

void DrawingComponentHolder::getSelectedBody(list<pair<int, const BodyInfo*>>& result)
{
    for (size_t cntItem = 0; cntItem < meshes.size(); ++cntItem)
    {
        if (!meshes[cntItem]->selected)
            continue;

        result.push_back(make_pair(cntItem, bodys[cntItem]));
    }
}

void DrawingComponentHolder::clearSelected()
{
    for (auto& mesh :  meshes)
    {
        mesh->selected = false;
    }
}

void DrawingComponentHolder::copySelectedData(const Vector3f& offset)
{
    //  model
    list<pair<int, const BodyInfo*>> lstGemIndex;
    getSelectedBody(lstGemIndex);

    //  copy
    Transform trs;
    trs.SetTranslate(1000.0 * offset);
    for (auto& gmInfo : lstGemIndex)
    {
        //  TODO 暂时只考虑
        Body bodyCopy = *(gmInfo.second->current);
        TriangleMesh meshCopy = *(gmInfo.second->getCurrentMesh());

        CGUtils::TransformBody(bodyCopy, trs);
        meshCopy.Transform(trs);

        bodys.push_back(new BodyInfo(bodyCopy, meshCopy));
        meshes.push_back(new MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, meshCopy), meshes[gmInfo.first]->color));
    }
}

void DrawingComponentHolder::deleteSelectedComponents()
{
    auto itrMeshItem = meshes.begin();
    auto itrBodyItem = bodys.begin();
    for (; itrMeshItem != meshes.end(); /*++itrMeshItem, ++itrBodyItem*/)
    {
        if (!(*itrMeshItem)->selected)
        {
            ++itrMeshItem, ++itrBodyItem;
            continue;
        }

        delete (*itrMeshItem);
        delete (*itrBodyItem);
        itrMeshItem = meshes.erase(itrMeshItem);
        itrBodyItem = bodys.erase(itrBodyItem);
    }
}

///////////////////////////////////////////////

void DrawingComponentHolder::GenerateLofting(Body& lofting)
{
    //  截面
    //  默认离散为18边型
    float radius = 80.f;
    Vector3f insert(3000.f, 1000.f, 0.f);

    int nEdge = 18;
    float angPer = PI * 2 / nEdge;
    Vector3f pt0(radius, 0.0, 0.0);
    vector<Vector3f> sectionPts{ pt0 + insert };
    for (size_t i = 1; i < nEdge; i++)
    {
        Transform rotate(Vector3f::BasicZ, i * angPer);
        Vector3f pt;
        Transform::MultPoint(rotate, pt0, pt);
        sectionPts.push_back(pt + insert);
    }

    //  path
    vector<Line> path;
    Vector3f pt1{ 3000.f, 1000.f, 1000.f };
    Vector3f pt2{ 3800.f, 1000.f, 1000.f };
    Vector3f pt3{ 3800.f, 2000.f, 1000.f };
    Vector3f pt4{ 3000.f, 1500.f, 2000.f };

    path.push_back(Line{ insert, pt1 });
    path.push_back(Line{ pt1, pt2 });
    path.push_back(Line{ pt2, pt3 });
    path.push_back(Line{ pt3, pt4 });

    CGUTILS::CGUtils::CreateLoftingSolid(sectionPts, path, lofting);
}

void DrawingComponentHolder::GenerateBody5(Body& body)
{
    Vector3f pt0(-400, -70, 200);
    Vector3f pt1(400, -70, 200);
    Vector3f pt2(400, 70, 200);
    Vector3f pt3(-400, 70, 200);
    Vector3f pt4(-400, -70, 600);
    Vector3f pt5(400, -70, 600);
    Vector3f pt6(400, 70, 600);
    Vector3f pt7(-400, 70, 600);

    CGUtils::GenerateCube(pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7, body);
}

void DrawingComponentHolder::GenerateBody4(Body& body)
{
    Vector3f pt0(-300, -100, 100);
    Vector3f pt1(300, -100, 100);
    Vector3f pt2(300, 100, 100);
    Vector3f pt3(-300, 100, 100);
    Vector3f pt4(-300, -100, 700);
    Vector3f pt5(300, -100, 700);
    Vector3f pt6(300, 100, 700);
    Vector3f pt7(-300, 100, 700);

    CGUtils::GenerateCube(pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7, body);
}

void DrawingComponentHolder::GenerateBody3(Body& body)
{
    Vector3f pt0(-100, -100, 100);
    Vector3f pt1(300, -100, 100);
    Vector3f pt2(300, 100, 100);
    Vector3f pt3(-100, 100, 100);
    Vector3f pt4(-100, -100, 700);
    Vector3f pt5(300, -100, 700);
    Vector3f pt6(300, 100, 700);
    Vector3f pt7(-100, 100, 700);

    CGUtils::GenerateCube(pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7, body);
}

void DrawingComponentHolder::GenerateBody2(Body& body)
{
    Vector3f pt0(-100, -100, -100);
    Vector3f pt1(300, -100, -100);
    Vector3f pt2(300, 100, -100);
    Vector3f pt3(-100, 100, -100);
    Vector3f pt4(-100, -100, 700);
    Vector3f pt5(300, -100, 700);
    Vector3f pt6(300, 100, 700);
    Vector3f pt7(-100, 100, 700);

    CGUtils::GenerateCube(pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7, body);
}

void DrawingComponentHolder::GenerateBody1(Body& body)
{
    Vector3f pt0(-200, -200, 0);
    Vector3f pt1(200, -200, 0);
    Vector3f pt2(200, 200, 0);
    Vector3f pt3(-200, 200, 0);
    Vector3f pt4(-200, -200, 800);
    Vector3f pt5(200, -200, 800);
    Vector3f pt6(200, 200, 800);
    Vector3f pt7(-200, 200, 800);

    CGUtils::GenerateCube(pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7, body);
}

void DrawingComponentHolder::GetCutFace(const Vector3f& origin, Face& cutFace)
{
    Vector3f pt0{ -250.0f, -180.0f, -300.0f };
    Vector3f pt1{ 300.0f, 120.0f, -300.0f };
    Vector3f dir0 = pt1 - pt0;
    dir0.Normalize();

    Vector3f pt2{ -250.0f, -180.0f, -300.0f };
    Vector3f pt3{ -200.0f, -180.0f, 300.0f };
    Vector3f dir1 = pt3 - pt2;
    dir1.Normalize();

    Vector3f vert0 = pt0 - dir0 * 1000.0f + origin;
    Vector3f vert1 = pt1 + dir0 * 1000.0f + origin;
    Vector3f vert2 = vert1 + dir1 * 2000.0f + origin;
    Vector3f vert3 = vert0 + dir1 * 2000.0f + origin;

    Line line0{ vert0, vert1 };
    Line line1{ vert1, vert2 };
    Line line2{ vert2, vert3 };
    Line line3{ vert3, vert0 };

    cutFace.outerLoop.lstLine.push_back(line0);
    cutFace.outerLoop.lstLine.push_back(line1);
    cutFace.outerLoop.lstLine.push_back(line2);
    cutFace.outerLoop.lstLine.push_back(line3);
}

void DrawingComponentHolder::BooleanOperateUsingCSGOctTree(TriangleMesh& meshA, TriangleMesh& meshB, TriangleMesh& meshInter, TriangleMesh& meshADiffB, TriangleMesh& meshBDiffA, TriangleMesh& meshUnion)
{
    Body cubeA, cubeB;
    GenerateBody1(cubeA);
    GenerateBody2(cubeB);

    Transform* trs = Transform::CreateIdentity();
    trs->SetTranslate(Vector3f(4000.f, 1000.f, 0.f));
    CGUtils::TransformBody(cubeA, *trs);
    CGUtils::TransformBody(cubeB, *trs);
    //Transform::Destroy(trs);

    //TriangleMesh meshA, meshB;
    CGUtils::TessellateBody(cubeA, meshA);
    CGUtils::TessellateBody(cubeB, meshB);
    meshA.SetId("meshA");
    meshB.SetId("meshB");

    CGUtils::BodyIntersect_CSGOctTree(meshA, meshB, meshInter);
    CGUtils::BodyDiff_CSGOctTree(meshA, meshB, meshADiffB);
    trs->SetTranslate(Vector3f(1000.f, 0.f, 0.f));
    meshADiffB.Transform(*trs);

    CGUtils::BodyDiff_CSGOctTree(meshB, meshA, meshBDiffA);
    trs->SetTranslate(Vector3f(2000.f, 0.f, 0.f));
    meshBDiffA.Transform(*trs);

    CGUtils::BodyUnion_CSGOctTree(meshA, meshB, meshUnion);
    trs->SetTranslate(Vector3f(3000.f, 0.f, 0.f));
    meshUnion.Transform(*trs);

    //  transform meshA and meshB
    trs->SetTranslate(Vector3f(0.f, 1000.f, 0.f));
    meshA.Transform(*trs);
    meshB.Transform(*trs);

    Transform::Destroy(trs);
}