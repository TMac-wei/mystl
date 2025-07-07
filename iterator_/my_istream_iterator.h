/**
 * @file      my_istream_iterator.h
 * @brief     [输入迭代器]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"
#include "utility_/my_forward.h"

#include <istream>

template<typename T, typename Distance = ptrdiff_t>
class istream_iterator {
public:
    using iterator_category = input_iterator_tag;
    using value_type = void;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = void;

    /// 默认构造函数，用于创建一个“尾后迭代器”，代表输入结束
    /// 对于 istream_iterator，流读完或出错会让 stream_ 变成 nullptr，此时就等价于尾后迭代器
    istream_iterator() : stream_(nullptr) {}

    /// 从输入流构造迭代器，指向当前流中的第一个值
    istream_iterator(std::istream &is) : stream_(&is) {
        /// ++*this 表示从流中读取第一个元素，并更新 value_，如果读失败（比如文件为空），stream_ 会被置空，迭代器自动变为“尾后状态
        ++*this;
    }

    /// * ->
    const T &operator*() const {
        return value_;
    }

    const T *operator->() const {
        return &value_;
    }

    /// 前置++
    istream_iterator &operator++() {
        /// 如果读取失败，则将stream_置nullptr
        if (stream_ && !(*stream_ >> value_)) {
            stream_ = nullptr;
        }
        return *this;
    }

    istream_iterator operator++(int) {
        istream_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    /// 只要一个迭代器 stream_ == nullptr，就代表“尾后状态”，可以用作终止判断
    friend bool operator==(const istream_iterator &lhs, const istream_iterator &rhs) {
        return lhs.stream_ == rhs.stream_;
    }

    friend bool operator!=(const istream_iterator &lhs, const istream_iterator &rhs) {
        return lhs != rhs;
    }

private:
    std::istream *stream_;
    T value_;
};

