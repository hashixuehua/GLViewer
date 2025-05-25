#include "glwindow.h"
#include "./ui_glwindow.h"

GLWindow::GLWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GLWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);
}

GLWindow::~GLWindow()
{
    delete ui;
}
