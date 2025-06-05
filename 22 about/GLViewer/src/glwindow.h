#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <string>
#include <QMainWindow>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QSplitter.h>
#include "AboutDialog.h"

using namespace std;

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
    void createStatusBar(void);

    void OnStatusMessageChanged(const string& message);

private slots:
    void website(void);
    void about(void);

private:
    Ui::GLWindow *ui;

    //  status bar
    QLabel* mousePosLabel;  // used for show mouse position
    QLabel* statusMessage;  // used for show status message

    //  about
    AboutDialog* aboutDlg;

};
#endif // GLWINDOW_H
