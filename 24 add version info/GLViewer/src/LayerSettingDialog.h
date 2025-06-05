#pragma once
#ifndef LAYERSETTINGDIALOG_H
#define LAYERSETTINGDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QDialog>
#include <QStandardItemModel>
#include <string>
#include <map>
#include "ViewerSetting.h"

using namespace std;

class LayerSettingDialog : public QDialog
{
    Q_OBJECT

protected:
    //QLabel m_label;
    QPushButton m_setCurButton;
    QPushButton m_addButton;
    QPushButton m_deleteButton;
    QTableView m_tableView;
    QStandardItemModel* m_tableViewModel;

public:
    explicit LayerSettingDialog(QWidget* parent = 0);
    void UpdateLayerData();

private:
    void InitTableView();
    void InitLayerData();

    int AddItem(const QColor& color, const QString& description, const string* ptrName);
    void RemoveItem(int rowIndex, const string& layerName);
    void UpdateItemColor(const QModelIndex& index, const QColor& color);

    void SetCurrent(int row);
    int getSelectedRow();

private slots:
    void onTableClicked(const QModelIndex& index);
    void deleteLayer();
    void addLayer();
    void updateActiveLayer();

};

#endif // LAYERSETTINGDIALOG_H

