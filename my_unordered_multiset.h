/**
 * @file      my_unordered_multiset.h
 * @brief     [unordered_multiset]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_hashtable.h"

namespace my {
    template<typename Key,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = MyAlloc<Key>>
    class unordered_multiset {
    public:
        using key_type = Key;
        using value_type = Key;
        using hash_table = hashtable<Key, Key, _Identify, std::false_type, Hash, Pred, Alloc>;
        using size_type = typename hash_table::size_type;
        using difference_type = typename hash_table::difference_type;
        using reference = typename hash_table::reference;
        using const_reference = typename hash_table::const_reference;
        using pointer = typename hash_table::pointer;
        using const_pointer = typename hash_table::const_pointer;
        using iterator = typename hash_table::iterator;
        using const_iterator = typename hash_table::const_iterator;

    public:
        explicit unordered_multiset(size_type n = 0) : table_(n) {}

        iterator begin() { return table_.begin(); }

        iterator end() { return table_.end(); }

        size_type size() const { return table_.size(); }

        size_type count(const key_type &key) const { return table_.count(key); }

        bool empty() const { return table_.empty(); }

        iterator insert(const value_type &value) { return table_.insert(value); }

        iterator insert(value_type &&value) { return table_.insert(my::move(value)); }

        template<typename... Args>
        iterator emplace(Args &&... args) {
            return table_.emplace(my::forward<Args>(args)...);
        }

        iterator find(const key_type &key) { return table_.find(key); }

        const_iterator find(const key_type &key) const { return table_.find(key); }

        void clear() { table_.clear(); }

        size_type erase(const key_type &key) { return table_.erase(key); }

        iterator erase(iterator it) {
            assert(it != end());
            return table_.erase(it);
        }

        void rehash(size_type n) { table_.rehash(n); }

        float load_factor() const { return table_.load_factor(); }

        void reserve(size_type n) { table_.reserve(n); }

        void swap(unordered_multiset &other) { table_.swap(other.table_); }

        bool operator==(const unordered_multiset &other) const { return table_ == other.table_; }

        bool operator!=(const unordered_multiset &other) const { return table_ != other.table_; }

    private:
        hash_table table_;
    };
}