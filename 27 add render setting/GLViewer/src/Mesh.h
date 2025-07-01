#ifndef MESH_H
#define MESH_H

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include "Setting.h"
//#include "assimp/Importer.hpp"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"

struct Vertex
{
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
    QVector3D Tangent;
    QVector3D Bitangent;
};

struct Texture
{
    QOpenGLTexture texture;
    QString type;
    QString path;
    Texture():texture(QOpenGLTexture::Target2D){
        texture.create();
        texture.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
        texture.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
        texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    }
};

class Mesh {
public:
    /*  网格数据  */
    std::vector<Vertex> vertices;               //顶点数据
    std::vector<unsigned int> indices;          //索引数组
    std::vector<Texture*> textures;             //纹理数据
    std::vector<unsigned int> edges;            //边线数据

    //bool onlyEdge = false;
    //QVector4D edgeColor;

    /*  函数  */
    //Mesh(QOpenGLFunctions_4_5_Core* glFunc) {};  //no use

    Mesh(QOpenGLFunctions_4_5_Core* glFunc);
    // constructor
    Mesh(QOpenGLFunctions_4_5_Core* glFunc, const std::vector<Vertex>& verts, const std::vector<unsigned int>& indis, const std::vector<Texture*>& texts, const std::vector<unsigned int>& edgs);
    ~Mesh();

    void Draw(QOpenGLShaderProgram& shader, bool wireFrame = false);
    //void Draw(QOpenGLShaderProgram* shader);
    void DrawWireframe(QOpenGLShaderProgram& shader);
    void setupMesh();
    QOpenGLVertexArrayObject* getVAO();

private:
    /*  渲染数据  */
    QOpenGLVertexArrayObject m_VAO, m_VAOEdge;
    QOpenGLBuffer m_VBO,m_EBO,m_EBOEdge;
    //opengl函数入口
    QOpenGLFunctions_4_5_Core* m_glFunc;
};

#endif // MESH_H
