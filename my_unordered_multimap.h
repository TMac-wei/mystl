/**
 * @file      my_unordered_multimap.h
 * @brief     [可重复插入相同的键值对]
 * @author    Weijh
 * @date      2025/7/6
 * @version   1.0
 */

#pragma once

#include "my_hashtable.h"
#include "my_pair.h"
#include "my_initializer_list.h"

/// 我的hashtable已经实现了重复插入键值，所以这里基于hashtable和unordered_multimap稍加修改为unodered_multimap
namespace my {
    /// 基于hashTable封装unordered_multimap
    template<typename Key,
            typename T,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = MyAlloc<my::pair<const Key, T>>>
    class unordered_multimap {
    public:
        /// 定义类型
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<const Key, T>;
        using hash_table = my::hashtable<Key, value_type, my::_Select1st, std::false_type, Hash, Pred, Alloc>;
        using size_type = typename hash_table::size_type;
        using difference_type = typename hash_table::difference_type;
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator;
    public:
        /// 主要成员方法
        /// 构造函数  rules of five
        explicit unordered_multimap(size_type n = 8) : table_(n) {}

        /// 非模板：支持 initializer_list 初始化
        unordered_multimap(std::initializer_list<std::pair<Key, T>> init)
                : table_(init.size()) {
            for (const auto &p: init) {
                insert(p);
            }
        }

        /// 模板版本：支持更广泛的兼容性（不影响上面）
        template<typename InputIt>
        unordered_multimap(InputIt first, InputIt last, size_type bucket_count = 10)
                : table_(bucket_count) {
            for (; first != last; ++first) {
                insert(value_type(*first));
            }
        }


        unordered_multimap(const unordered_multimap &) = default;

        unordered_multimap(unordered_multimap &&) noexcept = default;

        unordered_multimap &operator=(const unordered_multimap &) = default;

        unordered_multimap &operator=(unordered_multimap &&) noexcept = default;


        iterator begin() {
            return table_.begin();
        }

        iterator end() {
            return table_.end();
        }

        const_iterator begin() const {
            return table_.begin();
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

        /// size()
        size_type size() const {
            return table_.size();
        }

        bool empty() const {
            return table_.empty();
        }

        /// std::unordered_multimap中插入总是成功
        iterator insert(const value_type &value) {
            return table_.insert(value);
        }

        /// equal_range()函数
        my::pair<iterator, iterator> equal_range(const key_type &key) {
            return table_.equal_range(key);
        }

        /// const版本
        my::pair<const_iterator , const_iterator> equal_range(const key_type& key) const {
            return table_.equal_range(key);
        }


        iterator find(const key_type &key) {
            return table_.find(key);
        }

        size_type erase(const key_type &key) {
            return table_.erase(key);
        }

        iterator erase(iterator it) {
            return table_.erase(it);
        }

        void clear() {
            table_.clear();
        }

        /// 相同的键有多少个
        size_type count(const key_type &key) const {
            auto range = table_.equal_range(key);
            size_type cnt = 0;
            for (auto it = range.first; it != range.second; ++it) {
                ++cnt;
            }
            return cnt;
        }

        /// 是否包含 contains
        bool contains(const key_type &key) const {
            /// 如果找到key的迭代器不等于尾迭代器则表示包含
            return table_.find(key) != table_.end();
        }

        void swap(unordered_multimap &other) {
            table_.swap(other.table_);
        }

        bool operator==(const unordered_multimap &other) const {
            return table_ == other.table_;
        }

        bool operator!=(const unordered_multimap &other) const {
            return table_ != other.table_;
        }

    private:
        /// 成员变量主要还是哈希表
        hash_table table_;
    };

}