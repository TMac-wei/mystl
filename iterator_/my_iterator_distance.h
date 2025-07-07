/**
 * @file      my_iterator_difference.h
 * @brief     [计算两个迭代器之间的距离（即元素个数）]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 提取-》iterator_difference_t<Iter>：迭代器的 difference_type。
/// iterator_catorgory_t<Iter>：迭代器的分类标签类型
#include "my_iterator_traits.h"

namespace my{
    /// 针对不能进行last - first的迭代器类型计算两个元素之间的距离
/// InputIterator
    template<typename InputIterator>
    inline iterator_difference_t<InputIterator> distance_(InputIterator first, InputIterator last, input_iterator_tag) {
        /// 对于不能直接相减的迭代器类型直接采用遍历然后递增元素的方式
        iterator_difference_t<InputIterator> n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

/// 优先匹配 random_access_iterator_tag，因为 random_access_iterator_tag 是 input_iterator_tag 的子类
    template<typename RandomAccessIterator>
    inline iterator_difference_t<RandomAccessIterator> distance_(RandomAccessIterator first,
                                                                 RandomAccessIterator last,
                                                                 random_access_iterator_tag) {
        return last - first;
    }

    template<typename InputIterator>
    inline iterator_difference_t<InputIterator> distance(InputIterator first, InputIterator last) {
        return distance_(first, last, iterator_catorgory_t<InputIterator>());
    }
}
