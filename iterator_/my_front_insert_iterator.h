/**
 * @file      my_front_insert_iterator.h
 * @brief     [前向插入]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"
#include "../utility_/my_forward.h"

template <typename Container>
class front_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = void;

    /// 接受容器的引用，保存其地址
    explicit front_insert_iterator(Container &x) : container_(&x) {}

    /// 将值追加到容器末尾
    front_insert_iterator &operator=(const typename Container::value_type &value) {
        container_->push_back(value);
        return *this;
    }

    /// 支持完美转发
    template<typename T>
    front_insert_iterator &operator=(T &&value) {
        container_->push_back(forward<T>(value));
        return *this;
    }

    front_insert_iterator(const front_insert_iterator &) = default;

    front_insert_iterator &operator=(const front_insert_iterator &) = default;


    /// 操作符必须实现以兼容 STL 算法。它们本身无实际作用，返回自身即可
    front_insert_iterator &operator*() { return *this; }

    front_insert_iterator &operator++() { return *this; }

    front_insert_iterator &operator++(int) { return *this; }
private:
    Container* container_;
};

/// 通过back_insert()函数创建front_insert_iterator对象
template<typename Container>
inline front_insert_iterator<Container> front_inserter(Container &x) {
    return front_insert_iterator<Container>(x);
}
