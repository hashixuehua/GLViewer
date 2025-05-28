#include "DrawingViewerElemHolder.h"

DrawingViewerElemHolder::DrawingViewerElemHolder(QOpenGLFunctions_4_5_Core* glFunc) :m_glFunc(glFunc)
{
}

DrawingViewerElemHolder::~DrawingViewerElemHolder(){}


void DrawingViewerElemHolder::setup()
{
    setupWorkPlaneMesh();
    setupViewCube();
    setupMouse();
}

void DrawingViewerElemHolder::drawWorkPlane(QOpenGLShaderProgram& shader)
{
    auto itrFindD = mapName2VMesh.find(ViewerCache::defaultWorkPlane);
    auto itrFindC = mapName2VMesh.find(ViewerCache::currentWorkPlane);
    if (itrFindD == mapName2VMesh.end() || itrFindC == mapName2VMesh.end())
        return;

    auto& defaultWP = itrFindD->second;
    shader.setUniformValue("objectColor", defaultWP.color.GetWX(), defaultWP.color.GetWY(), defaultWP.color.GetWZ(), defaultWP.color.GetW());
    defaultWP.current->Draw(shader);

    auto& currentWP = itrFindC->second;
    shader.setUniformValue("objectColor", currentWP.color.GetWX(), currentWP.color.GetWY(), currentWP.color.GetWZ(), currentWP.color.GetW());
    currentWP.current->Draw(shader);
}

void DrawingViewerElemHolder::drawViewElement(QOpenGLShaderProgram& shader, const string& name)
{
    auto itrFind = mapName2VMesh.find(name);
    if (itrFind == mapName2VMesh.end())
        return;

    auto& viewCube = itrFind->second;

    string imagePath = "";
    if (name == ViewerCache::viewCube)
        imagePath = ":/viewcube.png";
    else if (name == ViewerCache::mouseLabel)
        imagePath = ":/rotateLabel.png";
    else
        throw exception("can not get valid image path.");

    auto textTexture = ViewerUtils::getOrCreateImageTexture(imagePath.c_str());
    textTexture->bind(0);

    shader.setUniformValue("texture_diffuse1", 0);

    //  viewCube
    shader.setUniformValue("objectColor", viewCube.color.GetWX(), viewCube.color.GetWY(), viewCube.color.GetWZ(), viewCube.color.GetW());
    viewCube.current->Draw(shader);

    textTexture->release();
}

const BodyInfo* DrawingViewerElemHolder::GetViewCubeBody()
{
    auto itrFind = mapName2VBody.find(ViewerCache::viewCube);
    if (itrFind == mapName2VBody.end())
        return nullptr;

    return &itrFind->second;
}

void DrawingViewerElemHolder::setupWorkPlaneMesh()
{
    //  cube
    WorkPlanePara workPlane[2]{
        WorkPlanePara(Vector3f::Zero, Vector3f::BasicZ, Vector3f::BasicX, Vector3f::BasicY, 20000.0f, 10, -1.8f),
        WorkPlanePara(Vector3f::Zero, Vector3f::BasicZ, Vector3f::BasicX, Vector3f::BasicY, 1000.0f, 4, 0.0f) };

    for (size_t cntItem = 0; cntItem < 2; cntItem++)
    {
        TriangleMesh mesh;
        GetWorkPlaneMesh(workPlane[cntItem], mesh);

        auto& name = cntItem == 0 ? ViewerCache::defaultWorkPlane : ViewerCache::currentWorkPlane;
        //mapName2VMesh[name] = MeshInfo(ConvertMesh(mesh));
        Vector3W color = name == ViewerCache::defaultWorkPlane ? Vector3W(1.0f, 1.0f, 1.0f, 0.8f) : Vector3W(0.6f, 0.85f, 0.92f, 0.2f);
        mapName2VMesh.insert(make_pair(name, MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color)));
    }
    //mapName2VMesh.at(ViewerCache::defaultWorkPlane).current->edgeColor = QVector4D(0.0, 0.0, 1.0, 1.0);
}

void DrawingViewerElemHolder::setupViewCube()
{
    vector<Vector3f> cubeVerts(4);
    CGLibUtils::getRectSection(Vector3f::Zero, Vector3f::BasicZ, 50.0f, -50.0f, &cubeVerts[0]);

    Body cube;
    CGUtils::GenerateCube(cubeVerts, Vector3f::BasicZ, 100.0f, cube);

    TriangleMesh cubeMesh;
    CGUtils::TessellateBody(cube, cubeMesh);

    //uv
    vector<Vector2f> lstUvs;
    lstUvs.push_back(Vector2f(0.0, 1.0));
    lstUvs.push_back(Vector2f(0.5, 1.0));
    lstUvs.push_back(Vector2f(0.5, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.0, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(1.0, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(1.0, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 1.0));
    lstUvs.push_back(Vector2f(1.0, 1.0));
    lstUvs.push_back(Vector2f(1.0, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.0, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 2.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.0, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(1.0, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(1.0, 0.0));
    lstUvs.push_back(Vector2f(0.5, 0.0));
    lstUvs.push_back(Vector2f(0.0, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 1.0 / 3.0));
    lstUvs.push_back(Vector2f(0.5, 0.0));
    lstUvs.push_back(Vector2f(0.0, 0.0));
    cubeMesh.SetUVs(lstUvs.size(), &lstUvs[0].U);

    auto mesh = CGLibUtils::ConvertMesh(m_glFunc, cubeMesh);
    mapName2VBody.insert(make_pair(ViewerCache::viewCube, BodyInfo(cube, cubeMesh)));
    mapName2VMesh.insert(make_pair(ViewerCache::viewCube, MeshInfo(mesh, Vector3W(1.0f, 1.0f, 1.0f, 1.0f))));
}

void DrawingViewerElemHolder::setupMouse()
{
    Vector3f pt0{ -2.f, -2.0f, 0.0f };
    Vector3f pt1{ 2.f, -2.0f, 0.0f };
    Vector3f pt2{ 2.0f, 2.f, 0.0f };
    Vector3f pt3{ -2.0f, 2.f, 0.0f };

    Vector3f nor0{ 0.0f, 0.0f, 1.0f };
    Vector3f nor1{ 0.0f, 0.0f, 1.0f };
    Vector3f nor2{ 0.0f, 0.0f, 1.0f };
    Vector3f nor3{ 0.0f, 0.0f, 1.0f };

    Face face;
    face.outerLoop.lstLine.push_back(Line{ pt0, pt1 });
    face.outerLoop.lstLine.push_back(Line{ pt1, pt2 });
    face.outerLoop.lstLine.push_back(Line{ pt2, pt3 });
    face.outerLoop.lstLine.push_back(Line{ pt3, pt0 });

    TriangleMesh cMesh;
    CGUtils::TesslateFace(face, cMesh);

    vector<Vector2f> lstUvs;
    lstUvs.push_back(Vector2f(0.0, 0.0));
    lstUvs.push_back(Vector2f(1.0, 0.0));
    lstUvs.push_back(Vector2f(1.0, 1.0));
    lstUvs.push_back(Vector2f(0.0, 1.0));
    cMesh.SetUVs(lstUvs.size(), &lstUvs[0].U);

    auto mesh = CGLibUtils::ConvertMesh(m_glFunc, cMesh);
    mapName2VMesh.insert(make_pair(ViewerCache::mouseLabel, MeshInfo(mesh, Vector3W(1.0f, 1.0f, 1.0f, 1.0f))));
}

void DrawingViewerElemHolder::GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh)
{
    Vector3f wrkPVs[4];
    CGLibUtils::getRectSection(Vector3f::Zero, Vector3f::BasicZ, workPlane.halfLength, workPlane.offset, wrkPVs);

    vector<Vector3f> sectionPts;
    for (size_t i = 0; i < 4; i++)
    {
        sectionPts.push_back(Vector3f(wrkPVs[i].X, wrkPVs[i].Y, wrkPVs[i].Z));
    }
    Vector3f normal = (sectionPts[1] - sectionPts[0]).CrossProduct(sectionPts[2] - sectionPts[1]);
    normal.Normalize();

    CGLibUtils::GenerateCubeMesh(sectionPts, normal, 0.3f, mesh);

    for (size_t i = 1; i < workPlane.gridCntPerEdge; i++)
    {
        Vector3f ptX0 = sectionPts[0] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * Vector3f::BasicY + 0.5 * Vector3f::BasicZ;
        Vector3f ptX1 = sectionPts[1] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * Vector3f::BasicY + 0.5 * Vector3f::BasicZ;
        mesh.AddVertex(ptX0);
        mesh.AddVertex(ptX1);
        mesh.AddEdge(Vector2ui(mesh.GetVertexCount() - 2, mesh.GetVertexCount() - 1));

        Vector3f ptY0 = sectionPts[0] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * Vector3f::BasicX + 0.5 * Vector3f::BasicZ;
        Vector3f ptY1 = sectionPts[3] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * Vector3f::BasicX + 0.5 * Vector3f::BasicZ;
        mesh.AddVertex(ptY0);
        mesh.AddVertex(ptY1);

        mesh.AddEdge(Vector2ui(mesh.GetVertexCount() - 2, mesh.GetVertexCount() - 1));
    }
}