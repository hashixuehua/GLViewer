#include "AboutDialog.h"
#include <QPalette>

AboutDialog::AboutDialog(QWidget* parent) :
    QDialog(parent), m_label(this), m_closeButton(this), m_plainTextEdit(this), m_urlBili(this), m_urlGithub(this)
{
    QPixmap pm(":/wechat.png");

    pm = pm.scaled(200, 200, Qt::KeepAspectRatio);

    m_label.setPixmap(pm);
    m_label.move(5, 5);
    m_label.resize(200, 200);

    QPalette p = m_plainTextEdit.palette();

    p.setColor(QPalette::Active, QPalette::Base, palette().color(QPalette::Active, QPalette::Window));
    p.setColor(QPalette::Inactive, QPalette::Base, palette().color(QPalette::Inactive, QPalette::Window));

    m_plainTextEdit.move(210, 30);
    m_plainTextEdit.resize(180, 150);
    m_plainTextEdit.setPalette(p);
    m_plainTextEdit.setFrameStyle(QFrame::NoFrame);
    m_plainTextEdit.setReadOnly(true);
    m_plainTextEdit.insertPlainText("GLViewer (1.5.2)\nPlatform:  Qt 6.8.0\n\nVideo:\nDownload:\n\nEmail:  heuwzl@163.com\nCopyright:  哈市雪花");

    m_urlBili.move(260, 80);
    m_urlBili.setOpenExternalLinks(true);//设置为true才能打开网页
    //m_urlBili.setText("<a style='color: blue; text-decoration: underline' href = www.cglib.net>www.cglib.net");
    m_urlBili.setText("<a style='color: blue; text-decoration: underline' href = https://space.bilibili.com/386294314>bilibili哔哩哔哩");
    m_urlBili.setAlignment(Qt::AlignLeft);

    m_urlGithub.move(275, 95);
    m_urlGithub.setOpenExternalLinks(true);//设置为true才能打开网页
    m_urlGithub.setText("<a style='color: blue; text-decoration: underline' href = https://github.com/hashixuehua/GLViewer-3D>github");
    m_urlGithub.setAlignment(Qt::AlignLeft);

    m_closeButton.setText("Close");
    m_closeButton.move(273, 190);
    m_closeButton.resize(100, 30);

    setFixedSize(390, 230);
    setWindowTitle("About GLViewer");

    connect(&m_closeButton, SIGNAL(clicked()), this, SLOT(close()));

}