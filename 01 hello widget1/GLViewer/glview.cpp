#include "glview.h"

GLView::GLView(QWidget *parent)
    : QOpenGLWidget{parent}
{}

void GLView::initializeGL()
{
    initializeOpenGLFunctions();
}
void GLView::resizeGL(int w, int h)
{

}
void GLView::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

