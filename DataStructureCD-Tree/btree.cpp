#include "btree.h"
#include "btreenode.h"
#include <QDebug>

BTree::BTree(QObject *parent, qint32 m) : QObject(parent), m(m)
{
    sentinel = new BTreeNode(this);
    new BTreeNode(sentinel);
    this->dumpObjectTree();
}

BTreeNode * BTree::root()
{
    return qobject_cast<BTreeNode *>(sentinel->children().first());
}

const BTreeNode * BTree::croot() const
{
    return qobject_cast<const BTreeNode *>(sentinel->children().first());
}

const QVector<QVariant> BTree::search(const QVariant &x)
{
    return root()->search(&x);
}

bool BTree::add(const QVariant &x)
{
    bool result = root()->add(&x, m);
    if (result)
        emit updated();
    return result;
}

bool BTree::del(const QVariant &x)
{
    bool result = root()->del(&x, m);
    if (result)
        emit updated();
    return result;
}
