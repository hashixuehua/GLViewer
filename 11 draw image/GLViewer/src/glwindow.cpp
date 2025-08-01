#include "glwindow.h"
#include "./ui_glwindow.h"

GLWindow::GLWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GLWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);

    createActions();
    createStatusBar();

    auto f1 = std::bind(&GLWindow::OnStatusMessageChanged, this, std::placeholders::_1);
    ui->openGLWidget->OnStatusMessageChanged = f1;
}

GLWindow::~GLWindow()
{
    delete ui;
}

void GLWindow::createActions(void)
{
    connect(ui->actionLine, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawLine()));
    connect(ui->actionArc, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawArc()));
    connect(ui->actionArc2, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawArc2()));
    connect(ui->actionCircle, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawCircle()));
    connect(ui->actionRectangle, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawRectangle()));
    connect(ui->actionImage, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawImage()));

}

void GLWindow::OnStatusMessageChanged(const string& message)
{
    statusMessage->setText(message.c_str());
}

void GLWindow::createStatusBar(void)
{
    QStatusBar* statusBar = this->statusBar(); // 获取状态栏

    // 1. 添加鼠标位置显示（从左侧添加）
    mousePosLabel = new QLabel("X: 0, Y: 0");
    statusBar->addWidget(mousePosLabel);

    // 2. 添加状态消息（短暂信息）
    statusMessage = new QLabel("GLViewer");
    statusBar->addWidget(statusMessage);

}
