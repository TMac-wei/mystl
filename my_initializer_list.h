/**
 * @file      my_initializer_list.h
 * @brief     [初始化列表]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <cstddef>

namespace my {
    template<typename T>
    class initializer_list {
    public:
        /// 类型定义--只读访问接口
        using value_type = T;
        using reference = const T &;
        using const_reference = const T &;
        using size_type = size_t;
        using iterator = const T *;
        using const_iterator = const T *;
        using pointer = const T *;
        using const_pointer = const T *;

        constexpr initializer_list() noexcept: begin_(nullptr), size_(0) {}

        constexpr initializer_list(const_iterator array, size_type size) noexcept
                : begin_(array), size_(size) {}

        constexpr size_type size() const noexcept {
            return size_;
        }

        const T *begin() const noexcept {
            return begin_;
        }

        const_iterator end() const noexcept {
            return begin_ + size_;
        }

    private:
        const_iterator begin_;
        size_type size_;
    };

    template<typename T>
    const T *begin(initializer_list<T> il) noexcept {
        return il.begin();
    }

    template<typename T>
    const T *end(initializer_list<T> il) noexcept {
        return il.end();
    }
}
