#pragma once
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDialog>

class AboutDialog : public QDialog
{
    Q_OBJECT

protected:
    QLabel m_label;
    QPushButton m_closeButton;
    QPlainTextEdit m_plainTextEdit;
    QLabel m_urlBili;
    QLabel m_urlGithub;

public:
    explicit AboutDialog(QWidget* parent = 0);


};

#endif // ABOUTDIALOG_H

