/**
 * @file      my_reverse_iterator.h
 * @brief     [反向迭代器]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"

namespace my {

template<typename Iterator>
class reverse_iterator {
public:
    /// 类型定义与类型萃取，利用 iterator_traits 萃取出类型属性
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;
    using iterator_type = Iterator;

    /// 构造和接口
    reverse_iterator() : current_() {}

    explicit reverse_iterator(Iterator x) : current_(x) {}

    reverse_iterator(const reverse_iterator &other) : current_(other.current_) {}

    /// 由其他迭代器类型构造
    template<typename Iter>
    reverse_iterator(const reverse_iterator<Iter> &x) : current_(x.base()) {}

    /// base() 返回底层的“正向迭代器”，等价于 std::reverse_iterator::base()
    iterator_type base() const {
        return current_;
    }

    /// operator*,因为时反向迭代器，所以跟正向迭代器的使用相反
    reference operator*() const {
        /// reverse_iterator 实际指向 current - 1 处的元素；
        /// current 表示“正向迭代器中将被访问的下一个元素”
        Iterator temp = current_;
        return *--temp;
    }

    /// 递增/递减运算符（正向操作反转）
    reverse_iterator &operator++() {
        --current_;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator temp = *this;
        --current_;
        return temp;
    }

    reverse_iterator &operator--() {
        ++current_;
        return *this;
    }

    reverse_iterator operator--(int) {
        reverse_iterator temp = *this;
        ++current_;
        return temp;
    }

    /// 算术操作符---与正向相反
    reverse_iterator operator+(difference_type n) const {
        /// 反向迭代器的+--》等于向前移动
        return reverse_iterator(current_ - n);
    }

    reverse_iterator &operator+=(difference_type n) {
        current_ -= n;
        return *this;
    }

    reverse_iterator operator-(difference_type n) const {
        /// 反向迭代器的+--》等于向前移动
        return reverse_iterator(current_ + n);
    }

    reverse_iterator operator-=(difference_type n) const {
        current_ += n;
        return *this;
    }

    /// 重载[] == !=
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    bool operator==(const reverse_iterator &x) const {
        return current_ == x.base();    /// 尽量不要直接访问裸指针
    }

    bool operator!=(const reverse_iterator &x) const {
        return current_ != x.base();    /// 尽量不要直接访问裸指针
    }

private:
    Iterator current_;      /// 目前的迭代器类型
};

/// 一些外接口
/// 所有比较操作都基于底层 base() 的比较
template<typename Iterator>
inline bool operator==(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() == y.base();
}

template<typename Iterator>
inline bool operator!=(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() != y.base();
}

template<typename Iterator>
inline bool operator<(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() > y.base();
}

template<typename Iterator>
inline bool operator<=(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() >= y.base();
}


template<typename Iterator>
inline bool operator>(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() < y.base();
}

template<typename Iterator>
inline bool operator>=(const reverse_iterator<Iterator> &x, const reverse_iterator<Iterator> &y) {
    return x.base() <= y.base();
}

/// 左加重载 和 -
/// operator- 计算两个 reverse_iterator 之间的距离
template<typename Iterator1, typename Iterator2>
inline typename reverse_iterator<Iterator1>::difference_type operator-(const reverse_iterator<Iterator1> &x,
                                                                       const reverse_iterator<Iterator1> &y) {
    return y.base() - x.base();
}

/// n + it 支持左加法（与右加等价）
template<typename Iterator>
inline reverse_iterator<Iterator> operator+(typename reverse_iterator<Iterator>::difference_type n,
                                            const reverse_iterator<Iterator> &x) {
    return reverse_iterator<Iterator>(x.base() - n);
}

}