#include <QApplication>
#include <QGuiApplication.h>
#include <QScreen.h>

#include "glwindow.h"
#include "ViewerSetting.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViewerSetting::devicePixelRatio = QGuiApplication::primaryScreen()->devicePixelRatio();
    GLWindow w;
    w.show();
    return a.exec();
}
