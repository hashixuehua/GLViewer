#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class GLWindow;
}
QT_END_NAMESPACE

class GLWindow : public QMainWindow
{
    Q_OBJECT

public:
    GLWindow(QWidget *parent = nullptr);
    ~GLWindow();

protected:
    void createActions(void);


private:
    Ui::GLWindow *ui;
};
#endif // GLWINDOW_H
