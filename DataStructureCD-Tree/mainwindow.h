#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "btree.h"
#include "btreenode.h"
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotTreeUpdated();
    void on_buttonAdd_clicked();
    void visualizeNode(void *parent, bool parentIsModel, QStandardItem *item, BTreeNode *node, const QString &leftBound, const QString &rightBound);

    void on_buttonRemove_clicked();

    void on_buttonExpand_clicked();

    void on_buttonCollapse_clicked();

    void on_buttonQuery_clicked();

    void on_buttonReset_clicked();


private:
    Ui::MainWindow *ui;
    BTree *tree;
    QStandardItemModel *model;
    bool expanding;

};

#endif // MAINWINDOW_H
