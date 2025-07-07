/**
 * @file      my_rbtree_map.h
 * @brief     [适用于map和multimap的红黑树结构]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */


#pragma once

#include <iostream>
#include <utility>

namespace my {

    enum Color {
        RED, BLACK
    };

// 节点结构体模板
    template<class K, class V>
    struct RBTreeNode {
        RBTreeNode(const std::pair<const K, V> &val = std::pair<const K, V>(), Color color = RED)
                : _pLeft(nullptr), _pRight(nullptr), _pParent(nullptr), _val(val), _color(color) {}

        RBTreeNode<K, V> *_pLeft;
        RBTreeNode<K, V> *_pRight;
        RBTreeNode<K, V> *_pParent;
        std::pair<const K, V> _val;
        Color _color;
    };

// 迭代器模板
    template<class K, class V>
    class RBTreeIterator {
        using Node = RBTreeNode<K, V>;
        using value_type = std::pair<const K, V>;  /// 封装只读类型
        using Self = RBTreeIterator<K, V>;
    public:
        RBTreeIterator(Node *pNode = nullptr, Node *head = nullptr)
                : _pNode(pNode), _pHead(head) {}

        /// std::map 中底层通常使用 pair<K, V>，然后通过自定义比较器保证 K 不被修改，而不是直接用 const K。
        std::pair<const K, V> &operator*() { return _pNode->_val; }

        std::pair<const K, V> *operator->() { return &(_pNode->_val); }

//        value_type& operator*() {
//            return *reinterpret_cast<value_type*>(&_pNode->_val);
//        }
//        value_type* operator->() {
//            return reinterpret_cast<value_type*>(&_pNode->_val);
//        }

        Self &operator++() {
            InCreament();
            return *this;
        }

        Self operator++(int) {
            Self tmp(*this);
            InCreament();
            return tmp;
        }

        bool operator!=(const Self &rhs) const { return _pNode != rhs._pNode; }

        bool operator==(const Self &rhs) const { return _pNode == rhs._pNode; }

    private:
        void InCreament() {
            if (_pNode->_pRight) {
                _pNode = _pNode->_pRight;
                while (_pNode->_pLeft) _pNode = _pNode->_pLeft;
            } else {
                Node *p = _pNode->_pParent;
                while (_pNode == p->_pRight) {
                    _pNode = p;
                    p = p->_pParent;
                }
                if (_pNode->_pRight != p)
                    _pNode = p;
            }
        }

    private:
        Node *_pNode;
        Node *_pHead;
    };

// 红黑树模板
    template<class K, class V>
    class RBTree {
        using Node = RBTreeNode<K, V>;
    public:
        using iterator = RBTreeIterator<K, V>;

        RBTree() : _size(0) {
            _pHead = new Node();
            _pHead->_pLeft = _pHead->_pRight = _pHead;
            _pHead->_color = RED;
        }

        ~RBTree() {
            _Destroy(GetRoot());
            delete _pHead;
        }

        std::pair<iterator, bool> InsertUnique(const std::pair<const K, V> &kv) {
            Node *&pRoot = _pHead->_pParent;
            if (!pRoot) {
                pRoot = new Node(kv, BLACK);
                pRoot->_pParent = _pHead;
                _UpdateHead();
                ++_size;
                return {iterator(pRoot, _pHead), true};
            }

            Node *cur = pRoot;
            Node *parent = _pHead;
            while (cur) {
                parent = cur;
                if (kv.first < cur->_val.first) cur = cur->_pLeft;
                else if (kv.first > cur->_val.first) cur = cur->_pRight;
                else return {iterator(cur, _pHead), false};
            }

            Node *newNode = new Node(kv);
            if (kv.first < parent->_val.first) parent->_pLeft = newNode;
            else parent->_pRight = newNode;
            newNode->_pParent = parent;

            _FixInsert(newNode);
            _UpdateHead();
            ++_size;
            return {iterator(newNode, _pHead), true};
        }

        iterator InsertMulti(const std::pair<const K, V> &kv) {
            Node *&pRoot = _pHead->_pParent;
            if (!pRoot) {
                pRoot = new Node(kv, BLACK);
                pRoot->_pParent = _pHead;
                _UpdateHead();
                ++_size;
                return iterator(pRoot, _pHead);
            }

            Node *cur = pRoot;
            Node *parent = _pHead;
            while (cur) {
                parent = cur;
                if (kv.first < cur->_val.first) cur = cur->_pLeft;
                else cur = cur->_pRight;
            }

            Node *newNode = new Node(kv);
            if (kv.first < parent->_val.first) parent->_pLeft = newNode;
            else parent->_pRight = newNode;
            newNode->_pParent = parent;

            _FixInsert(newNode);
            _UpdateHead();
            ++_size;
            return iterator(newNode, _pHead);
        }

        iterator Find(const K &key) {
            Node *cur = GetRoot();
            while (cur) {
                if (key < cur->_val.first) cur = cur->_pLeft;
                else if (key > cur->_val.first) cur = cur->_pRight;
                else return iterator(cur, _pHead);
            }
            return end();
        }

        size_t Count(const K &key) const {
            const Node *cur = GetRoot();
            size_t count = 0;
            while (cur) {
                if (key < cur->_val.first) {
                    cur = cur->_pLeft;
                } else {
                    if (!(key < cur->_val.first)) {
                        ++count;
                    }
                    cur = cur->_pRight;
                }
            }
            return count;
        }


        size_t Size() const { return _size; }

        iterator begin() { return iterator(_pHead->_pLeft, _pHead); }

        iterator end() { return iterator(_pHead, _pHead); }

        void Inorder() const {
            _Inorder(GetRoot());
            std::cout << "\n";
        }

    private:
        void _FixInsert(Node *node) {
            Node *parent = node->_pParent;

            while (parent != _pHead && parent->_color == RED) {
                Node *grandparent = parent->_pParent;

                if (parent == grandparent->_pLeft) {
                    Node *uncle = grandparent->_pRight;

                    if (uncle && uncle->_color == RED) {
                        // Case 1 - recoloring
                        parent->_color = BLACK;
                        uncle->_color = BLACK;
                        grandparent->_color = RED;
                        node = grandparent;
                        parent = node->_pParent;
                    } else {
                        if (node == parent->_pRight) {
                            // Case 2 - Left rotate
                            _RotateLeft(parent);
                            std::swap(node, parent);
                        }
                        // Case 3 - Right rotate
                        parent->_color = BLACK;
                        grandparent->_color = RED;
                        _RotateRight(grandparent);
                    }
                } else {
                    Node *uncle = grandparent->_pLeft;

                    if (uncle && uncle->_color == RED) {
                        parent->_color = BLACK;
                        uncle->_color = BLACK;
                        grandparent->_color = RED;
                        node = grandparent;
                        parent = node->_pParent;
                    } else {
                        if (node == parent->_pLeft) {
                            _RotateRight(parent);
                            std::swap(node, parent);
                        }
                        parent->_color = BLACK;
                        grandparent->_color = RED;
                        _RotateLeft(grandparent);
                    }
                }
            }

            GetRoot()->_color = BLACK;
        }

        void _RotateLeft(Node *x) {
            Node *y = x->_pRight;
            x->_pRight = y->_pLeft;
            if (y->_pLeft) y->_pLeft->_pParent = x;

            y->_pParent = x->_pParent;
            if (x == x->_pParent->_pLeft) {
                x->_pParent->_pLeft = y;
            } else if (x == x->_pParent->_pRight) {
                x->_pParent->_pRight = y;
            } else {
                _pHead->_pParent = y; // x 是根节点
            }

            y->_pLeft = x;
            x->_pParent = y;
        }

        void _RotateRight(Node *x) {
            Node *y = x->_pLeft;
            x->_pLeft = y->_pRight;
            if (y->_pRight) y->_pRight->_pParent = x;

            y->_pParent = x->_pParent;
            if (x == x->_pParent->_pLeft) {
                x->_pParent->_pLeft = y;
            } else if (x == x->_pParent->_pRight) {
                x->_pParent->_pRight = y;
            } else {
                _pHead->_pParent = y;
            }

            y->_pRight = x;
            x->_pParent = y;
        }

        Node *&GetRoot() { return _pHead->_pParent; }

        const Node *GetRoot() const { return _pHead->_pParent; }

        void _UpdateHead() {
            Node *root = GetRoot();
            if (!root) {
                _pHead->_pLeft = _pHead->_pRight = _pHead;
                return;
            }
            Node *left = root, *right = root;
            while (left->_pLeft) left = left->_pLeft;
            while (right->_pRight) right = right->_pRight;
            _pHead->_pLeft = left;
            _pHead->_pRight = right;
        }


        void _Destroy(Node *node) {
            if (!node) return;
            _Destroy(node->_pLeft);
            _Destroy(node->_pRight);
            delete node;
        }

        void _Inorder(const Node *node) const {
            if (!node) return;
            _Inorder(node->_pLeft);
            std::cout << node->_val.first << ":" << node->_val.second << " ";
            _Inorder(node->_pRight);
        }

    private:
        Node *_pHead;
        size_t _size;
    };

} // namespace my
