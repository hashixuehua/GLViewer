#ifndef MODEL_H
#define MODEL_H
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_4_5_Core>

class Model
{
public:
    Model(QOpenGLFunctions_4_5_Core* glFunc);
    ~Model();

    void Draw(QOpenGLShaderProgram& shader);

private:
    // 顶点的数据
    float vertices[18] = {
        -0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f,
        0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f,
        0.0f,  0.5f, 0.f, 0.f, 0.f, 1.f
    };

    unsigned int indices[3]{ 0, 1, 2 };

    QOpenGLFunctions_4_5_Core* m_glFunc;

    QOpenGLVertexArrayObject m_VAO;
    QOpenGLBuffer m_VBO, m_EBO;

};

#endif // MODEL_H
