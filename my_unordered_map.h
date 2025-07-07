/**
 * @file      my_unordered_map.h
 * @brief     [unordered_map复现]
 * @author    Weijh
 * @date      2025/7/6
 * @version   1.0
 */

#pragma once

#include "my_hashtable.h"
#include "memory_/my_allocator.h"
#include "my_pair.h"
#include "my_initializer_list.h"

namespace my {
    /// 基于hashTable封装unordered_map
    template<typename Key,
            typename T,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = MyAlloc<my::pair<const Key, T>>>
    class unordered_map {
    public:
        /// 定义类型
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<const Key, T>;
        using hash_table = my::hashtable<Key, value_type, my::_Select1st, std::true_type, Hash, Pred, Alloc>;
        using size_type = typename hash_table::size_type;
        using difference_type = typename hash_table::difference_type;
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator;
    public:
        /// 主要成员方法
        /// 构造函数  rules of five
        explicit unordered_map(size_type n = 8) : table_(n) {}

        unordered_map(my::initializer_list<value_type> init) {
            for (const auto &v: init) {
                insert(v);
            }
        }

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

        /// std::unordered_map中返回std::pair<iterator, bool>表示是否插入成功
        std::pair<iterator, bool> insert(const value_type &value) {
            return table_.insert(value);
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

        void swap(unordered_map &other) {
            table_.swap(other.table_);
        }

        bool operator==(const unordered_map &other) const {
            return table_ == other.table_;
        }

        bool operator!=(const unordered_map &other) const {
            return table_ != other.table_;
        }

        /// 重载[]
        T &operator[](const key_type &key) {
            /// 由于insert()返回std::pair<iterator, bool>，所以这里使用auto [it, inserted]分别接收插入返回的新迭代器，和是否插入成功
            auto [it, inserted] = table_.insert(std::make_pair(key, mapped_type()));
            return it->second;
        }

        /// 重载at(i)
        T &at(const key_type &key) {
            iterator it = table_.find(key);
            /// 如果要插入的键不在map范围内
            if (it == table_.end()) {
                throw std::out_of_range("unordered_map::at");
            }
            return it->second;
        }

        /// const 版本
        const T &at(const key_type &key) const {
            const_iterator it = table_.find(key);
            /// 如果要插入的键不在map范围内
            if (it == table_.end()) {
                throw std::out_of_range("unordered_map::at");
            }
            return it->second;
        }

    private:
        /// 成员变量主要还是哈希表
        hash_table table_;
    };
}

