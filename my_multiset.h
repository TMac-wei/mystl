/**
 * @file      my_multiset.h
 * @brief     [my_multiset]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_rbtree.h"

namespace my {
    template<typename T>
    class multiset {
        using tree_type = RBTree<T>;
    public:
        using key_type = T;
        using value_type = T;
        using size_type = size_t;
        using iterator = typename tree_type::iterator;

        multiset() = default;

        template<typename InputIt>
        multiset(InputIt first, InputIt last) {
            for (; first != last; ++first) {
                insert(*first);
            }
        }

        iterator begin() { return tree_.begin(); }
        iterator end() { return tree_.end(); }

        bool empty() const { return size_ == 0; }
        size_type size() const { return size_; }

        /// 插入（允许重复）
        iterator insert(const value_type& val) {
            tree_.Insert(val);  /// 重复也插入
            ++size_;
            return find(val);
        }

        void clear() {
            tree_ = tree_type();  /// 重置整个树
            size_ = 0;
        }

        /// 遍历查找第一个值匹配的位置
        iterator find(const value_type& val) {
            for (auto it = begin(); it != end(); ++it) {
                if (*it == val)
                    return it;
            }
            return end();
        }

        /// 返回值的出现次数
        size_type count(const value_type& val) {
            size_type cnt = 0;
            for (auto it = begin(); it != end(); ++it)
                if (*it == val) ++cnt;
            return cnt;
        }

        void swap(multiset& other) {
            std::swap(tree_, other.tree_);
            std::swap(size_, other.size_);
        }

        bool operator==(const multiset& rhs) const {
            if (size_ != rhs.size_) return false;
            auto it1 = begin(), it2 = rhs.begin();
            while (it1 != end() && it2 != rhs.end()) {
                if (!(*it1 == *it2)) return false;
                ++it1;
                ++it2;
            }
            return true;
        }

        bool operator!=(const multiset& rhs) const {
            return !(*this == rhs);
        }

        void inorder() const {
            tree_.Inorder();
            std::cout << '\n';
        }

    private:
        tree_type tree_;
        size_type size_ = 0;
    };

}

