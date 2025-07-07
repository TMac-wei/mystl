/**
 * @file      my_map.h
 * @brief     [基于红黑树封装map]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_rbtree_map.h"
#include <utility>
#include <initializer_list>

namespace my {

// ============================ my::map ============================
    template<typename K, typename V>
    class map {
    public:
        using value_type = std::pair<K, V>;
        using node_value_type = std::pair<K, V>;
        using iterator = typename RBTree<K, V>::iterator;

        map() = default;

        template<typename InputIt>
        map(InputIt first, InputIt last) {
            for (; first != last; ++first) {
                insert(*first);
            }
        }

        map(std::initializer_list<value_type> ilist) : map(ilist.begin(), ilist.end()) {}

        std::pair<iterator, bool> insert(const value_type &val) {
            return _tree.InsertUnique(val);
        }

        void insert_or_assign(const K& key, const V& val) {
            auto [it, inserted] = _tree.InsertUnique({key, val});
            if (!inserted)
                (*it).second = val;
        }


        iterator find(const K &key) {
            return _tree.Find(key);
        }

        V &operator[](const K &key) {
            auto [it, inserted] = _tree.InsertUnique({key, V()});
            return (*it).second;
        }

        size_t size() const {
            return _tree.Size();
        }

        iterator begin() { return _tree.begin(); }

        iterator end() { return _tree.end(); }

        void inorder() const {
            _tree.Inorder();
        }

    private:
        RBTree<K, V> _tree;
    };

// ============================ my::multimap ============================
    template<typename K, typename V>
    class multimap {
    public:
        using value_type = std::pair<const K, V>;
        using iterator = typename RBTree<K, V>::iterator;

        multimap() = default;

        template<typename InputIt>
        multimap(InputIt first, InputIt last) {
            for (; first != last; ++first) {
                insert(*first);
            }
        }

        multimap(std::initializer_list<value_type> ilist) : multimap(ilist.begin(), ilist.end()) {}

        iterator insert(const value_type &val) {
            return _tree.InsertMulti(val);
        }

        iterator find(const K &key) {
            return _tree.Find(key);
        }

        size_t count(const K &key) const {
            return _tree.Count(key);
        }

        size_t size() const {
            return _tree.Size();
        }

        iterator begin() { return _tree.begin(); }

        iterator end() { return _tree.end(); }

        void inorder() const {
            _tree.Inorder();
        }

    private:
        RBTree<K, V> _tree;
    };

}
