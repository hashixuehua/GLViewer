
#include "mesh.h"
//#include <QtOpenGLExtensions/QOpenGLExtensions>

Mesh::Mesh(QOpenGLFunctions_4_5_Core* glFunc)
    : m_glFunc(glFunc)
    , m_VBO(QOpenGLBuffer::VertexBuffer)
    , m_EBO(QOpenGLBuffer::IndexBuffer)
    , m_EBOEdge(QOpenGLBuffer::IndexBuffer)
{

}

Mesh::Mesh(QOpenGLFunctions_4_5_Core* glFunc, const std::vector<Vertex>& verts, const std::vector<unsigned int>& indis, const std::vector<Texture*>& texts,
    const std::vector<unsigned int>& edgs)
    : /*edgeColor(0.0f, 1.0f, 1.0f, 1.0f)
    , */m_glFunc(glFunc)
    , m_VBO(QOpenGLBuffer::VertexBuffer)
    , m_EBO(QOpenGLBuffer::IndexBuffer)
    , m_EBOEdge(QOpenGLBuffer::IndexBuffer)
{
    this->vertices = verts;
    this->indices = indis;
    this->textures = texts;
    this->edges = edgs;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

Mesh::~Mesh()
{
    m_VAO.destroy();
    m_VAOEdge.destroy();
    m_VBO.destroy();
    m_EBO.destroy();
    m_EBOEdge.destroy();
}

void Mesh::DrawWireframe(QOpenGLShaderProgram& shader)
{
    shader.setUniformValue("objectColor", Setting::edgeLineColor);
    m_glFunc->glLineWidth(Setting::edgeLineWidth);
    QOpenGLVertexArrayObject::Binder bind2(&m_VAOEdge);
    m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(edges.size()), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(QOpenGLShaderProgram &shader, bool wireFrame/* = false*/)
{
    if (wireFrame)
    {
        DrawWireframe(shader);
        return;
    }

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); ++i) {
        m_glFunc->glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        QString number;
        QString name = textures[i]->type;
        if (name == "texture_diffuse")
            number = QString::number(diffuseNr++);
        else if (name == "texture_specular")
            number = QString::number(specularNr++);
        else if (name == "texture_normal")
            number = QString::number(normalNr++);
        else if (name == "texture_height")
            number = QString::number(heightNr++);
        textures[i]->texture.bind();
        shader.setUniformValue((name + number).toStdString().c_str(), i);
    }
    // 画网格
    QOpenGLVertexArrayObject::Binder bind(&m_VAO);
    m_glFunc->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // 画边线
    //shader.setUniformValue("objectColor", Setting::edgeLineColor/*edgeColor*/);
    //QOpenGLVertexArrayObject::Binder bind2(&m_VAOEdge);
    //m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(edges.size()), GL_UNSIGNED_INT, 0);
}

//void Mesh::Draw(QOpenGLShaderProgram *shader)
//{
//    if (!onlyEdge)
//    {
//        unsigned int diffuseNr = 1;
//        unsigned int specularNr = 1;
//        unsigned int normalNr = 1;
//        unsigned int heightNr = 1;
//
//        for (unsigned int i = 0; i < textures.size(); ++i) {
//            m_glFunc->glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
//            // 获取纹理序号（diffuse_textureN 中的 N）
//            QString number;
//            QString name = textures[i]->type;
//            if (name == "texture_diffuse")
//                number = QString::number(diffuseNr++);
//            else if (name == "texture_specular")
//                number = QString::number(specularNr++);
//            else if (name == "texture_normal")
//                number = QString::number(normalNr++);
//            else if (name == "texture_height")
//                number = QString::number(heightNr++);
//            textures[i]->texture.bind();
//            shader->setUniformValue((name + number).toStdString().c_str(), i);
//        }
//        // 画网格
//        QOpenGLVertexArrayObject::Binder bind(&m_VAO);
//        m_glFunc->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//    }
//
//    // 画边线
//    //  Test  重新构造边线VAO
//    shader->setUniformValue("objectColor", 1.0f, 1.0f, 1.0f, 0.8f);
//    //m_glFunc->glLineWidth(2.0f);
//    //vector<unsigned int> lineArray{0, 1, 2, 3, 4, 5};
//    //int temp = static_cast<unsigned int>(indices.size()) / 2;
//    QOpenGLVertexArrayObject::Binder bind2(&m_VAOEdge);
//    m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(edges.size()), GL_UNSIGNED_INT, 0);
//    //glBindVertexArray(VAOEdge);
//    //glDrawElements(GL_LINES, static_cast<unsigned int>(edges.size()), GL_UNSIGNED_INT, 0);
//}

/*!
 * \brief Mesh::setupMesh 绑定顶点和索引
 */
void Mesh::setupMesh()
{

    m_VAO.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_VAO);

    m_VBO.create();
    m_EBO.create();

    m_VBO.bind();
    m_VBO.allocate(vertices.data(),vertices.size() * sizeof(Vertex));

    m_EBO.bind();
    m_EBO.allocate(indices.data(),indices.size() * sizeof(unsigned int));


    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    m_glFunc->glEnableVertexAttribArray(2);
    m_glFunc->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    m_glFunc->glEnableVertexAttribArray(3);
    m_glFunc->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    m_glFunc->glEnableVertexAttribArray(4);
    m_glFunc->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    //  edge VAO EBO
    m_VAOEdge.create();
    QOpenGLVertexArrayObject::Binder vaoBinderEg(&m_VAOEdge);

    m_EBOEdge.create();
    m_VBO.bind();

    //glGenVertexArrays(1, &VAOEdge);
    //glGenBuffers(1, &EBOEdge);
    //glBindVertexArray(VAOEdge);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if (!edges.empty())
    {
        m_EBOEdge.bind();
        m_EBOEdge.allocate(edges.data(), edges.size() * sizeof(unsigned int));

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOEdge);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(unsigned int), &edges[0], GL_STATIC_DRAW);
    }

    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
}

QOpenGLVertexArrayObject *Mesh::getVAO()
{
    return &m_VAO;
}
