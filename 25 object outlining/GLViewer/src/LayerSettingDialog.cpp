#include "LayerSettingDialog.h"
#include <QPalette>
#include <QHeaderView>
#include <qcolordialog.h>
#include <qmessagebox.h>
#include "LayerData.h"

LayerSettingDialog::LayerSettingDialog(QWidget* parent) :
    QDialog(parent), /*m_label(this),*/ m_setCurButton(this), m_addButton(this), m_deleteButton(this), m_tableView(this), m_tableViewModel(0)
{
    InitTableView();

    setFixedSize(390, 230);
    setWindowTitle("Layer Setting");

    m_setCurButton.setText("Active");
    m_setCurButton.move(20, 200);
    m_setCurButton.resize(80, 20);
    connect(&m_setCurButton, SIGNAL(clicked()), this, SLOT(updateActiveLayer()));

    m_deleteButton.setText("Delete");
    m_deleteButton.move(183, 200);
    m_deleteButton.resize(80, 20);
    connect(&m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteLayer()));

    m_addButton.setText("Add");
    m_addButton.move(283, 200);
    m_addButton.resize(80, 20);
    connect(&m_addButton, SIGNAL(clicked()), this, SLOT(addLayer()));

}

void LayerSettingDialog::SetCurrent(int rowIndex)
{
    m_tableViewModel->setItem(rowIndex, 0, new QStandardItem("Yes"));
    m_tableViewModel->item(rowIndex, 0)->setTextAlignment(Qt::AlignCenter);

    QString layerName = m_tableViewModel->item(rowIndex, 1)->text();
    LayerCache::setCurrentLayer(layerName.toStdString());

    for (size_t i = 0; i < m_tableViewModel->rowCount(); i++)
    {
        if (i != rowIndex)
        {
            m_tableViewModel->setItem(i, 0, new QStandardItem(""));
            m_tableViewModel->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

int LayerSettingDialog::AddItem(const QColor& color, const QString& description, const string* ptrName)
{
    int rowIndex = m_tableViewModel->rowCount();
    m_tableViewModel->setItem(rowIndex, 0, new QStandardItem(""));
    std::string layerName = ptrName ? *ptrName : LayerCache::generateLayerName();

    m_tableViewModel->setItem(rowIndex, 1, new QStandardItem(layerName.c_str()));
    m_tableViewModel->setItem(rowIndex, 2, new QStandardItem(color.name()));
    m_tableViewModel->setItem(rowIndex, 3, new QStandardItem(description));

    m_tableViewModel->item(rowIndex, 0)->setTextAlignment(Qt::AlignCenter);
    m_tableViewModel->item(rowIndex, 1)->setTextAlignment(Qt::AlignCenter);
    m_tableViewModel->item(rowIndex, 2)->setTextAlignment(Qt::AlignCenter);
    m_tableViewModel->item(rowIndex, 3)->setTextAlignment(Qt::AlignCenter);
    //设置颜色
    m_tableViewModel->item(rowIndex, 2)->setBackground(QBrush(color));
    m_tableView.setRowHeight(rowIndex, 20);

    //当前设置只有颜色可编辑
    //m_tableViewModel->item(rowIndex, 1)->setFlags(Qt::ItemIsSelectable);

    Vector3i cgColor(color.red(), color.green(), color.blue());
    LayerCache::addLayer(LayerData(layerName, cgColor, description.toStdString()));

    return rowIndex;
}

void LayerSettingDialog::RemoveItem(int rowIndex, const string& layerName)
{
    m_tableViewModel->removeRow(rowIndex);

    LayerCache::removeLayer(layerName);
}

void LayerSettingDialog::UpdateItemColor(const QModelIndex& index, const QColor& color)
{
    if (m_tableViewModel->itemFromIndex(index)->background().color() == color)
        return;

    QString layerName = m_tableViewModel->item(index.row(), 1)->text();
    LayerCache::updateLayerColor(layerName.toStdString(), Vector3i(color.red(), color.green(), color.blue()));

    m_tableViewModel->itemFromIndex(index)->setBackground(QBrush(color));
    m_tableViewModel->itemFromIndex(index)->setText(color.name());
}

void LayerSettingDialog::InitTableView()
{
    //准备数据模型
    m_tableViewModel = new QStandardItemModel();
    m_tableViewModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Activated")));
    m_tableViewModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    m_tableViewModel->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Color")));
    m_tableViewModel->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("Description")));
    //利用setModel()方法将数据模型与QTableView绑定
    m_tableView.setModel(m_tableViewModel);
    m_tableView.move(10, 10);
    m_tableView.resize(360, 180);

    //设置列宽不可变动，即不能通过鼠标拖动增加列宽        
    m_tableView.horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_tableView.horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_tableView.horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_tableView.horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    //设置表格的各列的宽度值        
    m_tableView.setColumnWidth(0, 60);
    m_tableView.setColumnWidth(1, 80);
    m_tableView.setColumnWidth(2, 100);
    m_tableView.setColumnWidth(3, 120);

    //默认显示行头，如果你觉得不美观的话，我们可以将隐藏        
    m_tableView.verticalHeader()->hide();
    //设置选中时为整行选中        
    m_tableView.setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑        
    m_tableView.setEditTriggers(QAbstractItemView::NoEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    m_tableView.setContextMenuPolicy(Qt::CustomContextMenu);

    connect(&m_tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTableClicked(const QModelIndex&)));

    InitLayerData();
}

void LayerSettingDialog::InitLayerData()
{
    m_tableViewModel->removeRows(0, m_tableViewModel->rowCount());

    AddItem(QColor(255, 0, 0), "default", nullptr);
    SetCurrent(0);
}

void LayerSettingDialog::UpdateLayerData()
{
    m_tableViewModel->removeRows(0, m_tableViewModel->rowCount());

    int indexActive = 0;
    for (auto& data : LayerCache::getAllLayer())
    {
        auto& layer = data.second;
        int curIndex = AddItem(QColor(layer.color.X, layer.color.Y, layer.color.Z), QString::fromStdString(layer.description), &layer.name);
        if (layer.name == LayerCache::getCurrentLayer().name)
            indexActive = curIndex;
    }

    SetCurrent(indexActive);
}

void LayerSettingDialog::onTableClicked(const QModelIndex& index)
{
    if (!index.isValid() || index.column() != 2)
        return;

    QColor color = QColorDialog::getColor(Qt::white, this);
    if (!color.isValid())
        return;

    UpdateItemColor(index, color);

}

void LayerSettingDialog::updateActiveLayer()
{
    int curRow = getSelectedRow();
    if (curRow == -1)
        return;

    SetCurrent(curRow);
}

int LayerSettingDialog::getSelectedRow()
{
    //QModelIndex cur = m_tableView.selectionModel()->currentIndex();

    QModelIndexList list = m_tableView.selectionModel()->selectedIndexes();
    if (list.count() <= 0)
        return -1;
    QModelIndex cur = list.back();

    //auto cur = m_tableView.currentIndex();
    if (!cur.isValid())
        return -1;

    return cur.row();
}

void LayerSettingDialog::deleteLayer()
{
    int curRow = getSelectedRow();
    if (curRow == -1)
        return;

    if (curRow == 0)
    {
        QMessageBox::warning(this, tr("warnning"), tr("can't delete the deault layer."));
        return;
    }

    string layerName = m_tableViewModel->item(curRow, 1)->text().toStdString();
    if (layerName == LayerCache::getCurrentLayer().name)
    {
        QMessageBox::warning(this, tr("warnning"), tr("can't delete the activated layer."));
        return;
    }

    RemoveItem(curRow, layerName);
}

void LayerSettingDialog::addLayer()
{
    auto cur = m_tableView.currentIndex();
    if (!cur.isValid())
        AddItem(QColor(255,255,255), "", nullptr);
    else
        AddItem(m_tableViewModel->item(cur.row(), 2)->background().color(), "", nullptr);
    m_tableView.clearSelection();
}
