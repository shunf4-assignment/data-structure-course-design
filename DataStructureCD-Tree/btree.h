#ifndef BTREE_H
#define BTREE_H

#include <QObject>
#include "btreestub.h"

class BTree : public QObject
{
    Q_OBJECT
public:
    // 显式构造函数
    explicit BTree(QObject *parent, qint32 m);
    // 哨兵结点
    BTreeNode *sentinel;
    // 阶
    qint32 m;

    // 获取该树的根结点
    BTreeNode *root();
    const BTreeNode *croot() const;
    // 树中搜索
    const QVector<QVariant> search(const QVariant &x);
    // 树中添加键值
    bool add(const QVariant &x);
    // 树中删除键值
    bool del(const QVariant &x);


signals:
    // 信号函数，当树的内容更新时会释放该信号
    void updated();
public slots:
};

#endif // BTREE_H
