#include "DrawingViewerElemHolder.h"

DrawingViewerElemHolder::DrawingViewerElemHolder(QOpenGLFunctions_4_5_Core* glFunc) :
    m_glFunc(glFunc),
    VBORectSelect(QOpenGLBuffer::VertexBuffer),
    EBORectSelect(QOpenGLBuffer::IndexBuffer)
{
}

DrawingViewerElemHolder::~DrawingViewerElemHolder(){}


void DrawingViewerElemHolder::setup()
{
    setupWorkPlaneMesh();
    setupViewCube();
    setupMouse();
    setupSnap();
    //setupRectSelect();//方式一
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
    QVector4D edgeColorTemp = Setting::edgeLineColor;

    string imagePath = "";
    if (name == ViewerCache::viewCube)
        imagePath = ":/viewcube.png";
    else if (name == ViewerCache::mouseLabel)
        imagePath = ":/rotateLabel.png";
    else if (name == ViewerCache::snapLabel || name == ViewerCache::rectSelect)
    {
        imagePath = "";
        Setting::edgeLineColor = Setting::snapColor;
    }
    else
        throw exception("can not get valid image path.");

    QOpenGLTexture* textTexture = nullptr;
    if (imagePath != "")
    {
        textTexture = ViewerUtils::getOrCreateImageTexture(imagePath.c_str());
        textTexture->bind(0);
    }

    shader.setUniformValue("texture_diffuse1", 0);

    //  viewCube
    shader.setUniformValue("objectColor", viewCube.color.GetWX(), viewCube.color.GetWY(), viewCube.color.GetWZ(), viewCube.color.GetW());
    viewCube.current->Draw(shader);

    Setting::edgeLineColor = edgeColorTemp;
    if (imagePath != "")
        textTexture->release();
}

const BodyInfo* DrawingViewerElemHolder::GetViewCubeBody()
{
    auto itrFind = mapName2VBody.find(ViewerCache::viewCube);
    if (itrFind == mapName2VBody.end())
        return nullptr;

    return &itrFind->second;
}

void DrawingViewerElemHolder::setCurrentWorkPlane(const Vector3f& planePt, const Vector3f& planeNormal, float offset)
{
    Vector3f localX, localY;
    CGUtils::getLocalXY(planeNormal, localX, localY);

    TriangleMesh mesh;
    GetWorkPlaneMesh(WorkPlanePara(planePt, planeNormal, localX, localY, 1000.0f, 4, offset), mesh);

    Vector3W color = Vector3W(0.6f, 0.85f, 0.92f, 0.2f);
    mapName2VMesh.at(ViewerCache::currentWorkPlane) = MeshInfo(CGLibUtils::ConvertMesh(m_glFunc, mesh), color);
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

void DrawingViewerElemHolder::setupSnap()
{
    Arc2d arc(Vector3f::Zero, 100.0, 0.0, 2 * PI, false);
    vector<Vector3f> lstVert;
    for (size_t i = 0; i < 21; i++)
    {
        lstVert.push_back(arc.GetCurvePoint(i * 0.05));
    }

    list<Line> lstLine;
    CGLibUtils::getContinusLines(lstVert, lstLine);

    //Transform trs(Vector3f::Zero, Vector3f::BasicX, PI / 2.0);
    //CGUtils::TransformPolygon(lstLine, trs);

    TriangleMesh cMesh;
    for (auto& line : lstLine)
    {
        cMesh.AddVertex(line.pt0);
        cMesh.AddVertex(line.pt1);
        cMesh.AddNormal(Vector3f::BasicZ);
        cMesh.AddNormal(Vector3f::BasicZ);

        cMesh.AddEdge(Vector2ui(cMesh.GetVertexCount() - 2, cMesh.GetVertexCount() - 1));
    }

    auto mesh = CGLibUtils::ConvertMesh(m_glFunc, cMesh);
    //mesh->edgeColor = Setting::snapColor;
    mapName2VMesh.insert(make_pair(ViewerCache::snapLabel, MeshInfo(mesh, Vector3W(1.0f, 1.0f, 1.0f, 1.0f))));
}

//  方式一绘制框选的矩形框：预先setup，后不断的计算modelMatrix，绘制
void DrawingViewerElemHolder::setupRectSelect()
{
    //  以原点为中心的单位矩形
    Vector3f pt0(-500.f, -500.0f, 0.f);
    Vector3f pt1 = pt0 + 1000.0 * Vector3f::BasicX;
    Vector3f pt2 = pt1 + 1000.0 * Vector3f::BasicY;
    Vector3f pt3 = pt0 + 1000.0 * Vector3f::BasicY;

    list<Line> lstLine;
    lstLine.push_back(Line{ pt0, pt1 });
    lstLine.push_back(Line{ pt1, pt2 });
    lstLine.push_back(Line{ pt2, pt3 });
    lstLine.push_back(Line{ pt3, pt0 });

    TriangleMesh cMesh;
    for (auto& line : lstLine)
    {
        cMesh.AddVertex(line.pt0);
        cMesh.AddVertex(line.pt1);
        cMesh.AddNormal(Vector3f::BasicZ);
        cMesh.AddNormal(Vector3f::BasicZ);

        cMesh.AddEdge(Vector2ui(cMesh.GetVertexCount() - 2, cMesh.GetVertexCount() - 1));
    }

    auto mesh = CGLibUtils::ConvertMesh(m_glFunc, cMesh);
    //mesh->edgeColor = QVector4D(1.0, 0.0, 1.0, 1.0);
    mapName2VMesh.insert(make_pair(ViewerCache::rectSelect, MeshInfo(mesh, Vector3W(1.0f, 1.0f, 1.0f, 1.0f))));

}

//  方式二绘制框选矩形框
void DrawingViewerElemHolder::drawRectSelect(QOpenGLShaderProgram& shader, const vector<Vector3f>& rectData)
{
    rectSelectIndices.clear();
    setupRectSelect(rectData);

    //  Test  重新构造边线VAO
    shader.setUniformValue("objectColor", 0.0f, 0.1f, 1.0f, 0.8f);

    //shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));
    //shader.enableAttributeArray(0);
    //
    ////  法线
    //shader.setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat) * 3, 3, 6 * sizeof(GLfloat));
    //shader.enableAttributeArray(1);

    m_glFunc->glLineWidth(Setting::curveWidth);
    //vector<unsigned int> lineArray{0, 1, 2, 3, 4, 5};
    //int temp = static_cast<unsigned int>(indices.size()) / 2;
    //glBindVertexArray(VAOMouse);
    QOpenGLVertexArrayObject::Binder vaoBinder(&VAORectSelect);

    //glFunc->glDrawElements(GL_TRIANGLES, 3u/*indices.size()*/, GL_UNSIGNED_INT, 0/*&indices[0]*/);
    //m_glFunc->glDrawArrays(GL_TRIANGLES, 0, 3);
    m_glFunc->glDrawElements(/*GL_LINES*/GL_LINE_LOOP, static_cast<unsigned int>(rectSelectIndices.size()), GL_UNSIGNED_INT, 0);
    //m_glFunc->glDrawElements(GL_LINES, 4u, GL_UNSIGNED_INT, &indices[0]);

    //glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}

void DrawingViewerElemHolder::setupRectSelect(const vector<Vector3f>& rectData)
{
    vector<float> lstVert;
    lstVert.push_back(rectData[0].X);
    lstVert.push_back(rectData[0].Y);
    lstVert.push_back(rectData[0].Z);
    lstVert.push_back(0.f);
    lstVert.push_back(0.f);
    lstVert.push_back(1.f);

    lstVert.push_back(rectData[1].X);
    lstVert.push_back(rectData[1].Y);
    lstVert.push_back(rectData[1].Z);
    lstVert.push_back(0.f);
    lstVert.push_back(0.f);
    lstVert.push_back(1.f);

    lstVert.push_back(rectData[2].X);
    lstVert.push_back(rectData[2].Y);
    lstVert.push_back(rectData[2].Z);
    lstVert.push_back(0.f);
    lstVert.push_back(0.f);
    lstVert.push_back(1.f);

    lstVert.push_back(rectData[3].X);
    lstVert.push_back(rectData[3].Y);
    lstVert.push_back(rectData[3].Z);
    lstVert.push_back(0.f);
    lstVert.push_back(0.f);
    lstVert.push_back(1.f);

    //qDebug() << "Shader::SetUniformValue: Uniform location '{}' could not be found." << lstVert.size();

    rectSelectIndices.push_back(0);
    rectSelectIndices.push_back(1);
    rectSelectIndices.push_back(2);
    rectSelectIndices.push_back(3);

    // create buffers/arrays
    if (!VAORectSelect.isCreated())
        VAORectSelect.create();
    else
        VAORectSelect.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&VAORectSelect);
    if (!VBORectSelect.isCreated())
        VBORectSelect.create();
    else
        VBORectSelect.release();
    if (!EBORectSelect.isCreated())
        EBORectSelect.create();
    else
        EBORectSelect.release();

    VBORectSelect.bind();
    VBORectSelect.allocate(&lstVert[0], sizeof(float) * lstVert.size());

    EBORectSelect.bind();
    EBORectSelect.allocate(&rectSelectIndices[0], sizeof(unsigned int) * rectSelectIndices.size());

    //m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(0);
    //
    ////  法线
    //m_shader->setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat) * 3, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(1);

    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

}

void DrawingViewerElemHolder::GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh)
{
    Vector3f wrkPVs[4];
    CGLibUtils::getRectSection(workPlane.origin, workPlane.normal, workPlane.halfLength, workPlane.offset, wrkPVs);

    vector<Vector3f> sectionPts;
    for (size_t i = 0; i < 4; i++)
    {
        sectionPts.push_back(Vector3f(wrkPVs[i].X, wrkPVs[i].Y, wrkPVs[i].Z));
    }
    //Vector3f normal = (sectionPts[1] - sectionPts[0]).CrossProduct(sectionPts[2] - sectionPts[1]);
    //normal.Normalize();

    CGLibUtils::GenerateCubeMesh(sectionPts, workPlane.normal, 0.3f, mesh);

    for (size_t i = 1; i < workPlane.gridCntPerEdge; i++)
    {
        Vector3f ptX0 = sectionPts[0] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * workPlane.localY/*Vector3f::BasicY*/ + 0.5 * workPlane.normal/*Vector3f::BasicZ*/;
        Vector3f ptX1 = sectionPts[1] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * workPlane.localY/*Vector3f::BasicY*/ + 0.5 * workPlane.normal/*Vector3f::BasicZ*/;
        mesh.AddVertex(ptX0);
        mesh.AddVertex(ptX1);
        mesh.AddEdge(Vector2ui(mesh.GetVertexCount() - 2, mesh.GetVertexCount() - 1));

        Vector3f ptY0 = sectionPts[0] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * workPlane.localX/*Vector3f::BasicX*/ + 0.5 * workPlane.normal/*Vector3f::BasicZ*/;
        Vector3f ptY1 = sectionPts[3] + i * (2.f * workPlane.halfLength / workPlane.gridCntPerEdge) * workPlane.localX/*Vector3f::BasicX*/ + 0.5 * workPlane.normal/*Vector3f::BasicZ*/;
        mesh.AddVertex(ptY0);
        mesh.AddVertex(ptY1);

        mesh.AddEdge(Vector2ui(mesh.GetVertexCount() - 2, mesh.GetVertexCount() - 1));
    }
}