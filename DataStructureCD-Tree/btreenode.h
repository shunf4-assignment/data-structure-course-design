#ifndef BTREENODE_H
#define BTREENODE_H

#include <QObject>
#include <QList>
#include <QVector>
#include "btreestub.h"

class BTreeNode : public QObject
{
    Q_OBJECT

private:
    // 拆分结点的算法
    static void split(BTreeNode *toBeSplitted, qint32 m);
    // 在叶结点删除一个键值
    void delAtLeaf(qint32 m, qint32 searchIndex);
    // 处理结点缺少足够的键值的算法
    static void dealWithKeywordShortage(BTreeNode *me, qint32 m, qint32 keyWordBackup);
    // 插入键值的算法
    bool insertKey(const QVariant *x, BTreeNode *childNodeLeft, BTreeNode *childNodeRight, qint32 m, qint32 searchIndex = -1);
    // 获取父结点
    BTreeNode *parentNode();
    // 根据键值在本结点的索引来删除键值的算法
    void delByIndex(qint32 m, qint32 searchIndex);

public:
    // 显式的构造函数
    explicit BTreeNode(QObject *parentNode = nullptr);
    // 结点的键值数，等于 keyWords 的长度
    qint32 n;
    // 键值表
    QList<QVariant> keyWords;
    // 子结点表
    QList<BTreeNode *> childNodes;
    // 在以该结点为根的子树中查找
    const QVector<QVariant> search(const QVariant *x);
    // 在以该结点为根的子树中添加键值
    bool add(const QVariant *x, qint32 m);
    // 在以该结点为根的子树中删除键值
    bool del(const QVariant *x, qint32 m);


signals:
public slots:

};

#endif // BTREENODE_H
