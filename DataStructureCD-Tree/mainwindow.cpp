#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "btreenode.h"
#include "btree.h"

#include <QFileSystemModel>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QStandardItemModel>
#include <QStyle>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());

    model = new QStandardItemModel(ui->mainTreeView);
    ui->mainTreeView->setModel(model);
    model->setHorizontalHeaderLabels(QStringList() << "描述" << "n 或 keyword");
    ui->mainTreeView->header()->resizeSection(0, 200);

    ui->lineKeyword->setValidator(new QIntValidator(this));

    tree = new BTree(this, 3);

    ui->labelOrder->setText(QString::number(3));

    connect(tree, SIGNAL(updated()), this, SLOT(slotTreeUpdated()));

    connect(ui->lineKeyword, SIGNAL(returnPressed()), ui->buttonAdd, SLOT(click()));



    expanding = true;
    slotTreeUpdated();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonAdd_clicked()
{
    if (ui->lineKeyword->hasAcceptableInput())
    {
        if (!tree->add(QVariant(ui->lineKeyword->text().toInt())))
             QMessageBox::critical(this, "失败", "添加失败。应该是已有该结点。");
    }

    ui->lineKeyword->clear();
}

void MainWindow::visualizeNode(void *parent, bool parentIsModel, QStandardItem *item, BTreeNode *node, const QString &leftBound, const QString &rightBound)
{
    QStandardItemModel *parent_model;
    QStandardItem *parent_item;

    if (parentIsModel)
    {
        parent_model = reinterpret_cast<QStandardItemModel *>(parent);
        parent_model->setItem(item->index().row(), 1, new QStandardItem(QString::number(node->n)));
    }
    else
    {
        parent_item = reinterpret_cast<QStandardItem *>(parent);
        parent_item->setChild(item->index().row(), 1, new QStandardItem(QString::number(node->n)));
    }

    for (qint32 i = 0; i <= node->n; i++)
    {
        BTreeNode *childNode = node->childNodes.at(i);
        QString currLeftBound = i == 0 ? leftBound : QString::number(node->keyWords.at(i - 1).value<qint32>());
        QString currRightBound = i == node->n ? rightBound : QString::number(node->keyWords.at(i).value<qint32>());
        if (childNode)
        {
            QStandardItem *childItem = new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_DirIcon), "结点");
            item->appendRow(childItem);
            item->setChild(childItem->index().row(), 1, new QStandardItem(QString::number(childNode->n)));
            visualizeNode(item, false, childItem, childNode, currLeftBound, currRightBound);
        }
        else
        {
            QStandardItem *childItem = new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_BrowserStop), "搜索失败");
            item->appendRow(childItem);
            item->setChild(childItem->index().row(), 1, new QStandardItem(currLeftBound + " ~ " + currRightBound));
        }

        if (i != node->n)
        {
            QStandardItem *childItem = new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_ArrowRight), "键值");
            item->appendRow(childItem);
            item->setChild(childItem->index().row(), 1, new QStandardItem(currRightBound));
        }
    }

}

void MainWindow::slotTreeUpdated()
{
    model->removeRows(0, model->rowCount());
    qDebug() << ("Tree update");

    QStandardItem *rootNode = new QStandardItem(QApplication::style()->standardIcon(QStyle::SP_DirIcon), "根结点");

    model->appendRow(rootNode);


    visualizeNode(model, true, rootNode, tree->root(), "-∞", "+∞");

    if (expanding)
        ui->mainTreeView->expandAll();
    else
    {
        ui->mainTreeView->expandToDepth(0);
    }
}

void MainWindow::on_buttonRemove_clicked()
{
    if (ui->lineKeyword->hasAcceptableInput())
    {
        if (!tree->del(QVariant(ui->lineKeyword->text().toInt())))
            QMessageBox::critical(this, "失败", "删除失败。应该是未找到该结点。");
    }

    ui->lineKeyword->clear();
}

void MainWindow::on_buttonExpand_clicked()
{
    expanding = true;
    ui->mainTreeView->expandAll();
}

void MainWindow::on_buttonCollapse_clicked()
{
    expanding = false;
    ui->mainTreeView->collapseAll();
    ui->mainTreeView->expandToDepth(0);
}

void MainWindow::on_buttonQuery_clicked()
{
    if (ui->lineKeyword->hasAcceptableInput())
    {
        auto searchResult = tree->search(QVariant(ui->lineKeyword->text().toInt()));
        auto searchFound = searchResult[0].toBool();

        if (!searchFound)
            QMessageBox::critical(this, "失败", "查询失败。应该是未找到该结点。");
        else
            QMessageBox::information(this, "成功", "查询成功。");
    }

    ui->lineKeyword->clear();
}

void MainWindow::on_buttonReset_clicked()
{
    if (ui->lineKeyword->hasAcceptableInput())
    {
        qint32 order = ui->lineKeyword->text().toInt();

        if (order >= 3)
        {
            delete tree;
            tree = new BTree(this, order);
            connect(tree, SIGNAL(updated()), this, SLOT(slotTreeUpdated()));
            slotTreeUpdated();
            ui->labelOrder->setText(QString::number(order));
        }
    }


}
