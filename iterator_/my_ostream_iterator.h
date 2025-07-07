/**
 * @file      my_ostream_iterator.h
 * @brief     [输出迭代器]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

#include "my_iterator_traits.h"

#include <iostream>

template<typename T, typename charT = char, typename Traits = std::char_traits<charT>>
class ostream_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = void;

    ostream_iterator(std::ostream &os) : stream_(&os), delimiter_(nullptr) {}

    ostream_iterator(std::ostream &os, const charT *delimiter) : stream_(&os), delimiter_(delimiter) {}

    /// 进行输入
    ostream_iterator &operator=(const T &value) {
        *stream_ << value;
        if (delimiter_) {
            *stream_ << delimiter_;
        }
        return *this;
    }

    ostream_iterator &operator*() { return *this; }

    ostream_iterator &operator++() { return *this; }

    ostream_iterator &operator++(int) { return *this; }


private:
    std::ostream *stream_;     /// 输出流
    const charT *delimiter_;    /// 输出内容
};