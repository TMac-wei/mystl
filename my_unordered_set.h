/**
 * @file      my_unordered_set.h
 * @brief     [my_unordered_set复现]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_hashtable.h"

namespace my {

/// unordered_set 基于hash_table实现
    template<typename Key,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = MyAlloc<Key>>
    class unordered_set {
    public:
        /// 类型定义
        using key_type = Key;
        using value_type = Key;
        using hash_table = hashtable<Key, Key, _Identify, std::true_type, Hash, Pred, Alloc>;
        using size_type = typename hash_table::size_type;
        using difference_type = typename hash_table::difference_type;
        using reference = typename hash_table::reference;
        using const_reference = typename hash_table::const_reference;
        using pointer = typename hash_table::pointer;
        using const_pointer = typename hash_table::const_pointer;
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator;
    public:
        ///rules of five
//    unordered_set() = default;

        explicit unordered_set(size_type n = 0)
                : table_(n) {}

        /// 拷贝构造
        unordered_set(const unordered_set &other)
                : table_(other.table_) {}

        unordered_set &operator=(const unordered_set &other) {
            table_ = other.table_;
            return *this;
        }

        unordered_set(unordered_set &&other) noexcept
                : table_(std::move(other.table_)) {}

        unordered_set &operator=(unordered_set &&other) noexcept {
            if (this != &other) {
                table_ = std::move(other.table_);
            }
            return *this;
        }

        /// 迭代器范围构造函数
        template<typename Inputiterator>
        unordered_set(Inputiterator first, Inputiterator last, size_type n = 0)
                : table_(n) {
            for (; first != last; ++first) {
                insert(*first);
            }
        }

        ~unordered_set() = default;

        /// 迭代器函数
        iterator begin() {
            return table_.begin();
        }

        const_iterator begin() const {
            return table_.begin();
        }

        iterator end() {
            return table_.end();
        }

        const_iterator end() const {
            return table_.end();
        }

        const_iterator cbegin() const {
            return table_.cbegin();
        }

        const_iterator cend() const {
            return table_.cend();
        }

        /// 获取大小
        size_type size() const {
            return table_.size();
        }

        size_type count(const key_type &key) const {
            return table_.count(key);
        }

        /// empty
        bool empty() const {
            return table_.empty();
        }

        /// insert

        /// =插入：返回 pair<iterator, bool>，对齐 STL 行为
        std::pair<iterator, bool> insert(const value_type &value) {
            return table_.insert(value);
        }

        /// find
        iterator find(const key_type &key) {
            return table_.find(key);
        }

        const_iterator find(const key_type &key) const {
            return table_.find(key);
        }

        /// erase
        size_type erase(const key_type &key) {
            return table_.erase(key);
        }

        iterator erase(const_iterator it) {
            /// stl标准接口是传入const_iterator,但是我的底层是iterator，所以这里需要做转换
//            return table_.erase(iterator(it.current_, const_cast<hash_table *>(&table_)));
            return table_.erase(iterator(it));
        }

        iterator erase(iterator it) {
            return table_.erase(it);  /// 直接传到底层 hashtable::erase(iterator)
        }

        /// 范围删除
        iterator erase(const_iterator first, const_iterator last) {
            while (first != last) {
                first = erase(first);
            }
            /// 返回删除之后的迭代器所在位置
//            return iterator(first.current_, const_cast<hash_table *>(&table_));
            return iterator(const_cast<typename hash_table::node_type*>(first.current_), const_cast<hash_table *>(&table_));

        }

        /// clear()
        void clear() {
            table_.clear();
        }

        /// swap
        void swap(unordered_set &other) {
            table_.swap(other.table_);
        }

        /// load_factor()
        float load_factor() const {
            return table_.load_factor();
        }

        /// rehash
        void rehash(size_type n) {
            table_.rehash_(n);
        }

        /// operator ==  !=
        bool operator==(const unordered_set &other) const {
            return table_ == other.table_;
        }

        bool operator!=(const unordered_set &other) const {
            return table_ != other.table_;
        }


    private:
        /// 成员变量
        hash_table table_;
    };
}
