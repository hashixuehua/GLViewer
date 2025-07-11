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
    //  MSAA
    QGroupBox groupAntiAliasingGroup;
    QRadioButton rbMode0MSAA;
    QRadioButton rbMode1MSAA;
    QRadioButton rbMode2MSAA;

    //  edge line
    QGroupBox groupShowEdgeLine;
    QRadioButton rbPostProcessingAA4EdgeLine;

    //  other setting
    QGroupBox groupOther;
    QRadioButton rbScaleByMousePoint;

public:
    explicit RenderSettingDialog(QWidget* parent = 0);
    
private:
    void Init();

private slots:
    void MSAARadioChanged(bool checked);
    void EdgeLinePPAAChanged(bool checked);
    void ScaleByMouseHoverChanged(bool checked);

};

#endif // RENDERSETTINGDIALOG_H

