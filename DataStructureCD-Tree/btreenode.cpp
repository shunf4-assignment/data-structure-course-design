#include "btreenode.h"
#include "btree.h"
#include <QException>
#include <QDebug>

Q_DECLARE_METATYPE(const BTreeNode*)

BTreeNode::BTreeNode(QObject *parent) : QObject(parent)
{
    keyWords.clear();
    childNodes.clear();

    n = 0;

    childNodes.append(nullptr);
}


const QVector<QVariant> BTreeNode::search(const QVariant *x)
{


    if (n == 0)
    {
        QVector<QVariant> result;
        result.append(false);
        result.append(qVariantFromValue(this));
        result.append(0);
        return result;
    }

    // Binary search in this node
    qint32 iBeg = 0;
    qint32 iEnd = n - 1;

    while(iBeg <= iEnd)
    {
        auto iMid = (iBeg + iEnd) / 2;
        if (keyWords.at(iMid) == (*x))
        {
            QVector<QVariant> result;
            result.append(true);
            result.append(qVariantFromValue(this));
            result.append(iMid);
            return result;
        }
        else if (keyWords.at(iMid) > (*x))
        {
            iEnd = iMid - 1;
            continue;
        }
        else if (keyWords.at(iMid) < (*x))
        {
            iBeg = iMid + 1;
            continue;
        }
    }

    BTreeNode *nextNode;
    assert(iEnd + 1 == iBeg);

    nextNode = childNodes.at(iBeg);

    if (nextNode == nullptr)
    {
        QVector<QVariant> result;
        result.append(false);
        result.append(QVariant::fromValue(this));
        result.append(iBeg);
        return result;
    }
    else
    {
        return nextNode->search(x);
    }
}

bool BTreeNode::add(const QVariant *x, qint32 m)
{
    auto searchResult = search(x);
    auto searchFound = searchResult[0].toBool();
    auto searchNode = searchResult[1].value<BTreeNode *>();
    auto searchIndex = searchResult[2].toInt();

    if (searchFound)
    {
        return false;
    }

    qDebug() << "Add : " << ("searchIndex " + QString::number(searchIndex)) ;

    searchNode->insertKey(x, nullptr, nullptr, m, searchIndex);
    return true;
}

bool BTreeNode::insertKey(const QVariant *x, BTreeNode *childNodeLeft, BTreeNode *childNodeRight, qint32 m, qint32 searchIndex)
{
    if (searchIndex == -1)
    {
        // Binary search in this node
        qint32 iBeg = 0;
        qint32 iEnd = n - 1;

        while(iBeg <= iEnd)
        {
            auto iMid = (iBeg + iEnd) / 2;
            if (keyWords.at(iMid) == (*x))
            {
                return false;
            }
            else if (keyWords.at(iMid) > (*x))
            {
                iEnd = iMid - 1;
                continue;
            }
            else if (keyWords.at(iMid) < (*x))
            {
                iBeg = iMid + 1;
                continue;
            }
        }
        searchIndex = iBeg;
    }

    keyWords.insert(searchIndex, *x);
    childNodes.insert(searchIndex, childNodeLeft);
    childNodes.replace(searchIndex + 1, childNodeRight);

    n++;

    if (n == m)
    {
        BTreeNode::split(this, m);
    }

    return true;
}

BTreeNode *BTreeNode::parentNode()
{
    return qobject_cast<BTreeNode *>(parent());
}

void BTreeNode::split(BTreeNode *toBeSplitted, qint32 m)
{
    BTreeNode *newSibling;
    BTreeNode *parentNode;
    if (qobject_cast<BTree *>(toBeSplitted->parent()->parent()) != nullptr)
    {
        // this Node's parent is a sentinel Node (this Node is the root)
        auto newRoot = new BTreeNode(toBeSplitted->parent());
        newSibling = new BTreeNode(newRoot);
        toBeSplitted->setParent(newRoot);
        parentNode = newRoot;
    }
    else
    {
        newSibling = new BTreeNode(toBeSplitted->parent());
        parentNode = toBeSplitted->parentNode();
    }

    newSibling->keyWords.append(toBeSplitted->keyWords.mid((toBeSplitted->n + 1) / 2, m / 2));
    newSibling->childNodes.clear();
    newSibling->childNodes.append(toBeSplitted->childNodes.mid((toBeSplitted->n + 1) / 2, m / 2 + 1));

    newSibling->n = m / 2;

    parentNode->insertKey(&(toBeSplitted->keyWords[(toBeSplitted->n - 1) / 2]), toBeSplitted, newSibling, m);
    for (int i = 0; i < m / 2 + 1; i++)
    {
        toBeSplitted->keyWords.removeLast();
        BTreeNode *c = toBeSplitted->childNodes.last();
        if (c)
            c->setParent(newSibling);
        toBeSplitted->childNodes.removeLast();
    }

    toBeSplitted->n = (toBeSplitted->n - 1) / 2;
}

bool BTreeNode::del(const QVariant *x, qint32 m)
{
    auto searchResult = search(x);
    auto searchFound = searchResult[0].toBool();
    auto searchNode = searchResult[1].value<BTreeNode *>();
    auto searchIndex = searchResult[2].toInt();

    if (!searchFound)
        return false;

    searchNode->delByIndex(m, searchIndex);
    return true;
}

void BTreeNode::delByIndex(qint32 m, qint32 searchIndex)
{
    if (childNodes[0] == nullptr)
    {
        // Leaf node
        delAtLeaf(m, searchIndex);
    }
    else
    {
        BTreeNode *leafNode = childNodes[searchIndex + 1];
        while (leafNode->childNodes[0] != nullptr)
        {
            leafNode = leafNode->childNodes[0];
        }

        keyWords[searchIndex] = leafNode->keyWords[0];
        leafNode->delAtLeaf(m, 0);
    }
}

void BTreeNode::delAtLeaf(qint32 m, qint32 searchIndex)
{
    if (childNodes[0] != nullptr)
        throw QException();

    qint32 keyWordBackup = keyWords.takeAt(searchIndex).toInt();
    childNodes.removeAt(searchIndex);

    n--;

    dealWithKeywordShortage(this, m, keyWordBackup);
}

void BTreeNode::dealWithKeywordShortage(BTreeNode *me, qint32 m, qint32 keyWordBackup)
{
    if (me->n >= (m-1) / 2)
        return;

    if (qobject_cast<BTree *>(me->parent()->parent()) != nullptr)
        return;

    BTreeNode *myParentNode = me->parentNode();

    qint32 myValue;
    if (me->keyWords.empty())
        myValue = keyWordBackup;
    else
        myValue = me->keyWords.first().toInt();

    QVariant newKeyWordBackup;

    // Binary search in parent node
    qint32 iBeg = 0;
    qint32 iEnd = myParentNode->n - 1;

    while(iBeg <= iEnd)
    {
        auto iMid = (iBeg + iEnd) / 2;

        if (myParentNode->keyWords.at(iMid) > myValue)
        {
            iEnd = iMid - 1;
            continue;
        }
        else if (myParentNode->keyWords.at(iMid) <= myValue)
        {
            iBeg = iMid + 1;
            continue;
        }
    }

    qint32 index = iEnd;
    BTreeNode *leftSibling = (iEnd != -1) ? myParentNode->childNodes.at(iEnd) : nullptr;
    BTreeNode *rightSibling = (iBeg != myParentNode->n) ? myParentNode->childNodes.at(iBeg + 1) : nullptr;

    bool leftSiblingRich = leftSibling && leftSibling->n > (m-1) / 2;
    bool rightSiblingRich = rightSibling && rightSibling->n > (m-1) / 2;

    BTreeNode *survivedSibling;

    if (leftSiblingRich)
    {
        me->keyWords.push_front(myParentNode->keyWords[index]);
        me->childNodes.push_front(leftSibling->childNodes.takeLast());
        if (me->childNodes.first())
            me->childNodes.first()->setParent(me);
        (me->n)++;

        myParentNode->keyWords[index] = leftSibling->keyWords.takeLast();

        leftSibling->n--;
    }
    else if (rightSiblingRich)
    {
        me->keyWords.push_back(myParentNode->keyWords[index + 1]);
        me->childNodes.push_back(rightSibling->childNodes.takeFirst());
        if (me->childNodes.last())
            me->childNodes.last()->setParent(me);
        (me->n)++;

        myParentNode->keyWords[index + 1] = rightSibling->keyWords.takeFirst();

        rightSibling->n--;
    } else if (leftSibling)
    {
        // Combine with left
        leftSibling->keyWords.append(newKeyWordBackup = myParentNode->keyWords.takeAt(index));
        leftSibling->keyWords.append(me->keyWords);
        leftSibling->childNodes.append(me->childNodes);

        if (me->childNodes.first())
        {
            // Non-leaf
            for (auto p = me->childNodes.cbegin(); p != me->childNodes.cend(); p++)
            {
                (*p)->setParent(leftSibling);
            }
        }

        myParentNode->childNodes.takeAt(index + 1);

        me->setParent(nullptr);

        delete me;
        survivedSibling = leftSibling;

        survivedSibling->n = survivedSibling->keyWords.count();
        myParentNode->n--;

    } else if (rightSibling)
    {
        me->keyWords.append(newKeyWordBackup = myParentNode->keyWords.takeAt(index + 1));
        me->keyWords.append(rightSibling->keyWords);
        me->childNodes.append(rightSibling->childNodes);

        if (rightSibling->childNodes.first())
        {
            for (auto p = rightSibling->childNodes.cbegin(); p != rightSibling->childNodes.cend(); p++)
            {
                (*p)->setParent(me);
            }
        }

        myParentNode->childNodes.takeAt(index + 2);

        rightSibling->setParent(nullptr);

        delete rightSibling;
        survivedSibling = me;

        survivedSibling->n = survivedSibling->keyWords.count();
        myParentNode->n--;
    }

    if (!leftSiblingRich && !rightSiblingRich && qobject_cast<BTree *>(myParentNode->parent()->parent()) != nullptr && myParentNode->n == 0)
    {
        // this parent's parent is a sentinel Node (this parent is the root)
        survivedSibling->setParent(myParentNode->parent());
        myParentNode->setParent(nullptr);
        delete myParentNode;
    }
    else
    {
        dealWithKeywordShortage(myParentNode, m, newKeyWordBackup.toInt());
    }
}


