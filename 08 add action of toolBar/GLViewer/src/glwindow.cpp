#include "glwindow.h"
#include "./ui_glwindow.h"

GLWindow::GLWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GLWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);

    createActions();
}

GLWindow::~GLWindow()
{
    delete ui;
}

void GLWindow::createActions(void)
{
    connect(ui->actionLine, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawLine()));
}
