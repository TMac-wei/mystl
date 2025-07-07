/**
 * @file      my_set.h
 * @brief     [my_set]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_rbtree.h"

namespace my {

    template<typename T>
    class set {
        using tree_type = RBTree<T>;
    public:
        using key_type = T;
        using value_type = T;
        using size_type = size_t;
        using iterator = typename tree_type::iterator;

        /// 构造函数
        set() = default;

        template<typename InputIt>
        set(InputIt first, InputIt last) {
            for (; first != last; ++first) {
                insert(*first);
            }
        }

        /// 迭代器
        iterator begin() { return tree_.begin(); }

        iterator end() { return tree_.end(); }

        /// 容量
        bool empty() const { return size_ == 0; }

        size_type size() const { return size_; }

        /// 修改器
        std::pair<iterator, bool> insert(const value_type &val) {
            bool inserted = tree_.Insert(val);
            if (inserted) ++size_;
            return {find(val), inserted};
        }

        void clear() {
            tree_ = tree_type(); /// 直接重置红黑树
            size_ = 0;
        }

        size_type erase(const value_type &val) {
            /// 目前的红黑树不支持删除，这里先不实现
            return 0;
        }

        void swap(set &other) {
            std::swap(tree_, other.tree_);
            std::swap(size_, other.size_);
        }

        /// 查找
        iterator find(const value_type &val) {
            for (auto it = begin(); it != end(); ++it) {
                if (*it == val)
                    return it;
            }
            return end();
        }

        size_type count(const value_type &val) {
            return find(val) != end() ? 1 : 0;
        }

        /// 比较
        bool operator==(const set &rhs) const {
            if (size_ != rhs.size_) return false;
            auto it1 = begin(), it2 = rhs.begin();
            while (it1 != end() && it2 != rhs.end()) {
                if (!(*it1 == *it2)) return false;
                ++it1;
                ++it2;
            }
            return true;
        }

        bool operator!=(const set &rhs) const {
            return !(*this == rhs);
        }

        /// 打印调试（非 STL）
        void inorder() const {
            tree_.Inorder();
            std::cout << '\n';
        }

    private:
        tree_type tree_;
        size_type size_ = 0;
    };

}
