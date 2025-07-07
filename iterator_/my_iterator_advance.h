/**
 * @file      my_iterator_advance.h
 * @brief     [根据迭代器的类型（如 input、bidirectional、random access）对迭代器进行“前进”或“后退”的操作]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"

/// 根据迭代器的类型，为每一种迭代器实现专属的++ --操作
/// 如果是 input_iterator_tag 类型的迭代器（单向，只支持 ++），就只能通过不断地 ++it 来前进
template<typename InputIterator>
inline void advance_(InputIterator &it, iterator_difference_t<InputIterator> n, input_iterator_tag) {
    while (n--) {
        ++it;
    }
}

/// BidirectionalIterator 实现,支持前进（++）和后退（--）的迭代器
template<typename BidirectionIterator>
inline void
advance_(BidirectionIterator &it, iterator_difference_t<BidirectionIterator> n, bidirectional_iterator_tag) {
    /// 如果 n 为正，就向前移动；如果 n 为负，就向后移动
    if (n > 0) {
        while (n--) {
            ++it;
        }
    } else {
        while (n++) {
            --it;
        }
    }
}

/// RandomAccessIterator --> 最快的迭代器类型，支持 it += n 运算，这种操作效率远高于多次 ++ 或 --
template<typename RandomAccessIterator>
inline void
advance_(RandomAccessIterator &it, iterator_difference_t<RandomAccessIterator> n, random_access_iterator_tag) {
    it += n;
}

/// 通用接口,用户调用的主函数
/// 通过 iterator_catorgory_t<Iter> 来获取对应的标签类型，然后调用对应的 advance_ 重载函数
template<typename InputIterator>
inline void advance(InputIterator &it, iterator_difference_t<InputIterator> n) {
    advance_(it, n, iterator_catorgory_t<InputIterator>()); ///  iterator_catorgory_t<Iter> 来获取对应的标签类型
}