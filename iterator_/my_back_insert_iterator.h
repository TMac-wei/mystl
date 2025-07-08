/**
 * @file      my_back_insert_iterator.h
 * @brief     [尾插迭代器]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"
#include "utility_/my_forward.h"

/// 仅写入不读取，所以value_type、pointer、reference = void
template<typename Container>
class back_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = void;

    /// 接受容器的引用，保存其地址
    explicit back_insert_iterator(Container &x) : container_(&x) {}

    /// 将值追加到容器末尾
    back_insert_iterator &operator=(const typename Container::value_type &value) {
        container_->push_back(value);
        return *this;
    }

    /// 支持完美转发
    template<typename T>
    back_insert_iterator &operator=(T &&value) {
        container_->push_back(forward<T>(value));
        return *this;
    }

    back_insert_iterator(const back_insert_iterator &) = default;

    back_insert_iterator &operator=(const back_insert_iterator &) = default;


    /// 操作符必须实现以兼容 STL 算法。它们本身无实际作用，返回自身即可
    back_insert_iterator &operator*() { return *this; }

    back_insert_iterator &operator++() { return *this; }

    back_insert_iterator &operator++(int) { return *this; }

private:
    Container *container_;
};

/// 通过back_insert()函数创建back_insert_iterator对象
template<typename Container>
inline back_insert_iterator<Container> back_inserter(Container &x) {
    return back_insert_iterator<Container>(x);
}