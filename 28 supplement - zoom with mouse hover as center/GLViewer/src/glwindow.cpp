#include "glwindow.h"
#include "./ui_glwindow.h"
#include <QDesktopServices>

GLWindow::GLWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GLWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);

    createActions();
    createStatusBar();

    aboutDlg = new AboutDialog(this);

    auto f1 = std::bind(&GLWindow::OnStatusMessageChanged, this, std::placeholders::_1);
    ui->openGLWidget->OnStatusMessageChanged = f1;
}

GLWindow::~GLWindow()
{
    delete ui;
}

void GLWindow::createActions(void)
{
    // File
    connect(ui->actionRead, SIGNAL(triggered()), ui->openGLWidget, SLOT(readLinesFromFile()));
    connect(ui->actionSave, SIGNAL(triggered()), ui->openGLWidget, SLOT(saveLinesToFile()));

    // Draw
    connect(ui->actionLine, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawLine()));
    connect(ui->actionArc, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawArc()));
    connect(ui->actionArc2, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawArc2()));
    connect(ui->actionCircle, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawCircle()));
    connect(ui->actionRectangle, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawRectangle()));
    connect(ui->actionImage, SIGNAL(triggered()), ui->openGLWidget, SLOT(drawImage()));

    // Control
    connect(ui->actionRectSelect, SIGNAL(triggered()), ui->openGLWidget, SLOT(boxSelect()));
    connect(ui->actionCopy, SIGNAL(triggered()), ui->openGLWidget, SLOT(copy()));
    connect(ui->actionDelete, SIGNAL(triggered()), ui->openGLWidget, SLOT(deleteSelected()));
    connect(ui->actionShowOrHideLines, SIGNAL(triggered()), ui->openGLWidget, SLOT(showOrHideLines()));
    connect(ui->actionShowOrHideModel, SIGNAL(triggered()), ui->openGLWidget, SLOT(showOrHideModel()));

    // Setting
    connect(ui->actionWorkPlane, SIGNAL(triggered()), ui->openGLWidget, SLOT(setWorkPlane()));
    connect(ui->actionLayer, SIGNAL(triggered()), ui->openGLWidget, SLOT(layerSetting()));
    connect(ui->actionRender, SIGNAL(triggered()), ui->openGLWidget, SLOT(renderSetting()));
    connect(ui->actionWireframe, SIGNAL(triggered()), ui->openGLWidget, SLOT(wireFrame()));

    // Help
    connect(ui->actionWebsite, SIGNAL(triggered()), this, SLOT(website()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

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

void GLWindow::website(void)
{
    QDesktopServices::openUrl(QUrl("https://www.cglib.net", QUrl::TolerantMode));
}

void GLWindow::about(void)
{
    aboutDlg->exec();
}
