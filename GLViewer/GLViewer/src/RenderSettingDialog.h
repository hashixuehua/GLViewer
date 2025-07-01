#pragma once
#ifndef RENDERSETTINGDIALOG_H
#define RENDERSETTINGDIALOG_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QDialog>
#include <QStandardItemModel>
#include <string>
#include <map>
#include "ViewerSetting.h"
#include <QRadioButton>

using namespace std;

class RenderSettingDialog : public QDialog
{
    Q_OBJECT

protected:
    //QLabel m_label;
    QGroupBox groupAntiAliasingGroup;
    QRadioButton rbMode0MSAA;
    QRadioButton rbMode1MSAA;
    QRadioButton rbMode2MSAA;

    QGroupBox groupShowEdgeLine;
    QRadioButton rbPostProcessingAA4EdgeLine;

public:
    explicit RenderSettingDialog(QWidget* parent = 0);
    
private:
    void Init();

private slots:
    void MSAARadioChanged(bool checked);
    void EdgeLinePPAAChanged(bool checked);

};

#endif // RENDERSETTINGDIALOG_H

