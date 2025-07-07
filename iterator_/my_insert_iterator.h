/**
 * @file      my_insert_iterator.h
 * @brief     [插入迭代器]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"
#include "utility_/my_forward.h"

template<typename Container>
class insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = void;

    using container_type = Container;
    using iterator_type = typename Container::iterator;

    insert_iterator(Container &x, iterator_type it) : container_(x), iter_(it) {}

    /// 重载=
    insert_iterator &operator=(const typename Container::value_type &value) {
        iter_ = container_->insert(iter_, value);
        ++iter_;
        return *this;
    }

    /// 完美转发重载
    template<typename T>
    insert_iterator &operator=(T &&value) {
        iter_ = container_->insert(iter_, forward<T>(value));
        ++iter_;
        return *this;
    }

private:
    Container *container_;
    iterator_type iter_;
};

template<typename Container>
inline insert_iterator<Container> inserter(Container &x, typename Container::iterator it) {
    return insert_iterator<Container>(x, it);
}