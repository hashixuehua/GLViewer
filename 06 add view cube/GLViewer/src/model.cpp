#include "model.h"

Model::Model(QOpenGLFunctions_4_5_Core* glFunc):
    m_glFunc(glFunc),
    m_VBO(QOpenGLBuffer::VertexBuffer),
    m_EBO(QOpenGLBuffer::IndexBuffer)
{
    setupWorkPlaneMesh();
    setupViewCube();
}

Model::~Model()
{
    m_VAO.destroy();
    m_VBO.destroy();
    m_EBO.destroy();
}

void Model::Draw(QOpenGLShaderProgram &shader)
{
    if (!m_VAO.isCreated())
        m_VAO.create();
    else
        m_VAO.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_VAO);

    if (!m_VBO.isCreated())
        m_VBO.create();
    else
        m_VBO.release();
    if (!m_EBO.isCreated())
        m_EBO.create();
    else
        m_EBO.release();

    m_VBO.bind();
    m_VBO.allocate(vertices, sizeof(vertices));

    m_EBO.bind();
    m_EBO.allocate(indices, sizeof(indices));

    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    shader.setUniformValue("objectColor", 1.f, 0.2f, 0.2f, 1.f);

    QOpenGLVertexArrayObject::Binder bind(&m_VAO);
    m_glFunc->glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}

void Model::Draw2(QOpenGLShaderProgram& shader)
{
    m_glFunc->glLineWidth(2.0f);

    //  work plane
    //  由于工作平面有透明度，最后绘制
    drawWorkPlane(shader);

    Draw(shader);

}

void Model::drawWorkPlane(QOpenGLShaderProgram& shader)
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

void Model::DrawViewCube(QOpenGLShaderProgram& shader)
{
    auto itrFind = mapName2VMesh.find(ViewerCache::viewCube);
    if (itrFind == mapName2VMesh.end())
        return;

    auto& viewCube = itrFind->second;

    auto textTexture = ViewerUtils::getOrCreateImageTexture(":/viewcube.png");
    textTexture->bind(0);

    shader.setUniformValue("texture_diffuse1", 0);

    //  viewCube
    shader.setUniformValue("objectColor", viewCube.color.GetWX(), viewCube.color.GetWY(), viewCube.color.GetWZ(), viewCube.color.GetW());
    viewCube.current->Draw(shader);

    textTexture->release();
}

void Model::setupWorkPlaneMesh()
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
        mapName2VMesh.insert(make_pair(name, MeshInfo(ConvertMesh(mesh), color)));
    }
}

void Model::setupViewCube()
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

    auto mesh = ConvertMesh(cubeMesh);
    mapName2VBody.insert(make_pair(ViewerCache::viewCube, BodyInfo(cube, cubeMesh)));
    mapName2VMesh.insert(make_pair(ViewerCache::viewCube, MeshInfo(mesh, Vector3W(1.0f, 1.0f, 1.0f, 1.0f))));
}

void Model::GetWorkPlaneMesh(const WorkPlanePara& workPlane, TriangleMesh& mesh)
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

const BodyInfo* Model::GetViewCubeBody()
{
    auto itrFind = mapName2VBody.find(ViewerCache::viewCube);
    if (itrFind == mapName2VBody.end())
        return nullptr;

    return &itrFind->second;
}

shared_ptr<Mesh> Model::ConvertMesh(const TriangleMesh& mesh)
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
    return make_shared<Mesh>(m_glFunc, vertices, indices, textures, edges);
}
