/**
 * @file      my_rbtree.h
 * @brief     [my_rbtree]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <iostream>

using namespace std;
enum Color {
    RED, BLACK
};

// 节点结构体模板
template<class V>
struct RBTreeNode {
    RBTreeNode(const V &val = V(), Color color = RED)
            : _pLeft(nullptr), _pRight(nullptr), _pParent(nullptr), _val(val), _color(color) {}

    RBTreeNode<V> *_pLeft;
    RBTreeNode<V> *_pRight;
    RBTreeNode<V> *_pParent;
    V _val;
    Color _color;
};

// STL 迭代器
template<class V>
class RBTreeIterator {
    typedef RBTreeNode<V> Node;
    typedef RBTreeIterator<V> Self;
public:
    RBTreeIterator(Node *pNode = nullptr, Node *head = nullptr)
            : _pNode(pNode), _pHead(head) {}

    //让迭代器具有指针的功能
    V &operator*() {
        return _pNode->_val;
    }

    V *operator->() {
        return &(operator*());
    }

    //移动
    // 前置++
    Self operator++() {
        InCreament();
        return *this;
    }

    // 后置++
    Self operator++(int) {
        Self temp(*this);
        InCreament();
        return temp;
    }


    //移动
    // 前置--
    Self operator--() {
        DeCreament();
        return *this;
    }

    // 后置--
    Self operator--(int) {
        Self temp(*this);
        DeCreament();
        return temp;
    }


    //比较
    bool operator!=(const Self &s) const {
        return _pNode != s._pNode;
    }

    bool operator==(const Self &s) const {
        return _pNode == s._pNode;
    }


    void InCreament() {
        if (_pNode->_pRight) {
            _pNode = _pNode->_pRight;
            while (_pNode->_pLeft)
                _pNode = _pNode->_pLeft;
        } else {
            Node *pParent = _pNode->_pParent;
            while (pParent && _pNode == pParent->_pRight) {
                _pNode = pParent;
                pParent = _pNode->_pParent;
            }

            //_pNdoe在根节点的位置，并且根节点没有右孩子
            if (_pNode->_pRight != pParent)
                _pNode = _pNode->_pParent;
        }
    }

    void DeCreament() {
        /// 目前下面的这种实现方式，会让我在反向遍历的时候，漏掉一个最大值
        /**
        //_pNode在end，end--取到最大值
        // 如果当前是红黑树的end节点（哨兵），则跳到最大节点
        if (_pNode->_pParent->_pParent == _pNode && _pNode->_color == RED) {
            _pNode = _pNode->_pRight;
        }
        if (_pNode->_pLeft) {
            _pNode = _pNode->_pLeft;
            while (_pNode->_pRight) {
                _pNode = _pNode->_pRight;
            }

        } else {
            Node *pParent = _pNode->_pParent;
            while (_pNode == pParent->_pLeft) {
                _pNode = pParent;
                pParent = _pNode->_pParent;
            }
            _pNode = pParent;
        }*/

        /// 这里进行修改
        /// 判断是否为_pHead,即end()，如果是就跳到最大节点
        if (_pNode == _pHead) {
            _pNode = _pHead->_pRight;
            return;
        }

        /// 正常向左子树最大值前移
        if (_pNode->_pLeft) {
            _pNode = _pNode->_pLeft;
            while (_pNode->_pRight) {
                _pNode = _pNode->_pRight;
            }
        } else {
            Node *pParent = _pNode->_pParent;
            while (_pNode == pParent->_pLeft) {
                _pNode = pParent;
                pParent = pParent->_pParent;
            }
            _pNode = pParent;
        }
    }

private:
    Node *_pNode;
    Node *_pHead; /// 用于判断 end()、--end()
};

template<class V>
class RBTree {
    typedef RBTreeNode<V> Node;
public:
    typedef RBTreeIterator<V> iterator;

public:
    RBTree() {
        /**
         * 这里是构建一个PHead_哨兵节点
         *              字段                  含义
         *       _pHead->_pParent       指向整棵红黑树的根节点
         *       _pHead->_pLeft         指向红黑树的最小节点，用于 begin()
         *       _pHead->_pRight        指向红黑树的最大节点，用于 --end()
         *       _pHead                 本身  表示 end()，即遍历的边界
         * */
        _pHead = new Node;
        _pHead->_pLeft = _pHead;
        _pHead->_pRight = _pHead;
    }

    ~RBTree() {
        _Destroy(GetRoot()); // 释放整棵树节点（不包含 _pHead）
        delete _pHead;       // 释放哨兵节点
        _pHead = nullptr;
    }

    iterator begin() {
        // 最左节点
        return iterator(_pHead->_pLeft, _pHead); /// 最小节点
    }

    iterator end() {
        // 哨兵节点
        return iterator(_pHead, _pHead);  /// 哨兵节点
    }

    void Insert(const V &val) {
        Node *&pRoot = _pHead->_pParent;
        // 空树的情况
        if (nullptr == pRoot) {
            pRoot = new Node(val, BLACK);
            pRoot->_pParent = _pHead;
        } else {
            //按照二叉搜索树得性质插入结点
            //找待插入节点在二叉搜索树中的位置
            // 这里采用两个指针的方式，一个指针cur找到新节点需要插入的位置，
            // 然后另一个指针parent一直跟着当前节点，待找到位置后， 新创建节点的父节点即刚刚跟着的节点parent
            Node *pCur = pRoot;
            Node *pParent = _pHead;
            while (pCur) {
                pParent = pCur;
                if (val < pCur->_val)
                    pCur = pCur->_pLeft;
                else if (val > pCur->_val)
                    pCur = pCur->_pRight;
                else {
                    /// 在这里进行重复值的插入，如果等于就插入到右边子树上
                    pCur = pCur->_pRight;
                }

            }

            //插入新节点
            pCur = new Node(val);
            if (val < pParent->_val)
                pParent->_pLeft = pCur;
            else
                pParent->_pRight = pCur;
            pCur->_pParent = pParent;

            //pParent的颜色是红色，一定违反红黑树的性质
            // 红黑树的性质：不允许连续红色节点出现
            while (pParent != _pHead && RED == pParent->_color) {
                Node *grandFather = pParent->_pParent;
                if (pParent == grandFather->_pLeft) {
                    Node *uncle = grandFather->_pRight;
                    /***
                     * 情况一：叔叔节点存在且为红色
                     * 将parent与uncle节点改为黑色，并且将grandfater节点改为红色
                     * 更新pcur = pgrandfather, pParent = pCur->_pParent;
                     * 继续向上修复
                     *
                     * */
                    if (uncle && RED == uncle->_color) {
                        pParent->_color = BLACK;
                        uncle->_color = BLACK;
                        grandFather->_color = RED;
                        pCur = grandFather;
                        pParent = pCur->_pParent;
                    } else {
                        //情况二：叔叔节点不存在或者存在且为黑色
                        //情况三：pCur是pParent的右孩子
                        if (pCur == pParent->_pRight) {
                            RotateL(pParent);
                            swap(pParent, pCur);
                        }

                        //情况二：
                        grandFather->_color = RED;
                        pParent->_color = BLACK;
                        RotateR(grandFather);
                    }
                } else  // pParent == grandFather->_pRight  与 pParent == grandFather->_pLeft是对称的
                {
                    Node *uncle = grandFather->_pLeft;
                    if (uncle && RED == uncle->_color) {
                        pParent->_color = BLACK;
                        uncle->_color = BLACK;
                        grandFather->_color = RED;
                        pCur = grandFather;
                        pParent = pCur->_pParent;
                    } else {
                        if (pCur == pParent->_pLeft) {
                            RotateR(pParent);
                            swap(pParent, pCur);
                        }

                        pParent->_color = BLACK;
                        grandFather->_color = RED;
                        RotateL(grandFather);

                    }
                }

            }

        }
        _pHead->_pLeft = LeftMost();
        _pHead->_pRight = RightMost();
        /// 根节点的固有性质，染色为黑色
        pRoot->_color = BLACK;

    }

    void Inorder() const {
        return _Inorder(GetRoot());
    }

    void Inorder() {
        return _Inorder(GetRoot());
    }

    ///是否有效的红黑树,通过性质进行检查
    bool IsValidRBTree() {
        Node *pRoot = GetRoot();
        if (nullptr == pRoot)
            return true;

        ///性质二：
        if (BLACK != pRoot->_color) {
            cout << "违反性质二：根节点的颜色不是黑色" << endl;
            return false;
        }

        ///性质三：不能有相连的红色结点

        //获取一条路径（任何一条路径-->这里选择最左边的路径）中黑色结点的个数
        Node *pCur = pRoot;
        size_t blackCount = 0;
        while (pCur) {
            if (BLACK == pCur->_color)
                ++blackCount;
            pCur = pCur->_pLeft;
        }
        // 性质四：每条路径里面的黑色结点数目相同
        size_t pathCount = 0;
        return _IsValidRBTree(pRoot, blackCount, pathCount);
    }

private:
    void _Destroy(Node *root) {
        if (!root) return;
        _Destroy(root->_pLeft);
        _Destroy(root->_pRight);
        delete root;
    }

    bool _IsValidRBTree(Node *pRoot, size_t blackCount, size_t pathCount) {
        if (nullptr == pRoot)
            return true;

        if (pRoot->_color == BLACK)
            // 本条路径中的黑色节点数目
            pathCount++;

        Node *pParent = pRoot->_pParent;
        if (pParent != _pHead && RED == pParent->_color && pRoot->_color == RED) {
            cout << "违反了性质三，有连在一起的红色结点" << endl;
            return false;
        }

        if (nullptr == pRoot->_pLeft && pRoot->_pRight == nullptr) {
            if (pathCount != blackCount) {
                cout << "违反性质四：路径中黑色结点得个数不相同" << endl;
                return false;
            }
        }

        // 递归查询左右两个子树的情况
        return _IsValidRBTree(pRoot->_pLeft, blackCount, pathCount) &&
               _IsValidRBTree(pRoot->_pRight, blackCount, pathCount);
    }

    /**
     *  找到红黑树中的最左边的节点（最小的节点）
     * */
    Node *LeftMost() {
        Node *pCur = GetRoot();
        if (nullptr == pCur)
            return _pHead;
        while (pCur->_pLeft)
            pCur = pCur->_pLeft;
        return pCur;
    }

    /**
     *  找到红黑树中的最右的节点（最大的节点）
     * */
    Node *RightMost() {
        Node *pCur = GetRoot();
        /**
         *  如果返回的是nullptr说明没有实际节点，只有一个哨兵节点，则返回哨兵节点
         * */
        if (nullptr == pCur)
            return _pHead;
        while (pCur->_pRight)
            pCur = pCur->_pRight;
        return pCur;
    }


    /**
     *  RotateL左旋，和右旋RotateR是对称操作
     *  每次旋转都有三次断开和三次重连，要注意其中的一些空节点的判断
     * */
    void RotateL(Node *pParent) {
        Node *pSubR = pParent->_pRight;
        Node *pSubRL = pSubR->_pLeft;

        pParent->_pRight = pSubRL;
        if (pSubRL)
            pSubRL->_pParent = pParent;

        pSubR->_pLeft = pParent;
        Node *pPParent = pParent->_pParent;
        pParent->_pParent = pSubR;
        pSubR->_pParent = pPParent;

        // 如果nparent已经是根节点，则左旋之后，新的根节点pSubR的父节点是哨兵节点
        if (pPParent == _pHead) {
            _pHead->_pParent = pSubR;
        } else {
            // 如果pParent不是根节点，则新的pSubR就会取代原始的nParent的位置
            if (pParent == pPParent->_pLeft)
                pPParent->_pLeft = pSubR;
            else
                pPParent->_pRight = pSubR;
        }

    }

    // 左右旋是对称的
    void RotateR(Node *pParent) {
        Node *pSubL = pParent->_pLeft;
        Node *pSubLR = pSubL->_pRight;

        pParent->_pLeft = pSubLR;
        if (pSubLR)
            pSubLR->_pParent = pParent;

        pSubL->_pRight = pParent;
        Node *pPParent = pParent->_pParent;
        pParent->_pParent = pSubL;
        pSubL->_pParent = pPParent;

        if (_pHead == pPParent) {
            _pHead->_pParent = pSubL;
        } else {
            if (pParent == pPParent->_pLeft)
                pPParent->_pLeft = pSubL;
            else
                pPParent->_pRight = pSubL;
        }
    }

    // 中序遍历
    void _Inorder(Node *pRoot) {
        if (pRoot) {
            _Inorder(pRoot->_pLeft);
            cout << pRoot->_val << " ";
            _Inorder(pRoot->_pRight);
        }
    }

    void _Inorder(const Node *pRoot) const {
        if (pRoot) {
            _Inorder(pRoot->_pLeft);
            std::cout << pRoot->_val << " ";
            _Inorder(pRoot->_pRight);
        }
    }


private:
    Node *&GetRoot() {
        return _pHead->_pParent;
    }

    const Node *GetRoot() const {
        return _pHead->_pParent;
    }

private:
    Node *_pHead;
};
