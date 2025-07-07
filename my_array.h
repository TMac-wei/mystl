/**
 * @file      my_array.h
 * @brief     [array复现]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "my_type_traits.h"
#include "my_iterator.h"
#include "my_utility.h"

namespace my {
    /// 复现array
    template<typename T, size_t N>
    struct array {
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;

        using iterator = value_type *;
        using const_iterator = const value_type *;
        using reverse_iterator = my::reverse_iterator<iterator>;
        using const_reverse_iterator = my::reverse_iterator<const_iterator>;

        /// array的底层就是大小为N的固定数组
        value_type elems[N];

        /// 以下是相关接口，均在编译器完成计算
        constexpr reference operator[](size_type index) {
            return elems[index];
        }

        constexpr const_reference operator[](size_t index) const {
            return elems[index];
        }

        /// 找到头元素和尾巴元素
        constexpr reference front() {
            return elems[0];
        }

        constexpr const_reference front() const {
            return elems[0];
        }

        constexpr reference back() {
            return elems[N - 1];
        }

        constexpr const_reference back() const {
            return elems[N - 1];
        }

        /// 获取当前指针
        constexpr pointer data() noexcept {
            return elems;
        }

        constexpr const_pointer data() const noexcept {
            return elems;
        }

        /// 获取迭代器 begin/cbegin
        constexpr iterator begin() noexcept {
            return iterator(elems);
        }

        constexpr const_iterator begin() const noexcept {
            return const_iterator(elems);
        }

        constexpr const_iterator cbegin() const noexcept {
            return const_iterator(elems);
        }

        constexpr iterator end() noexcept {
            return iterator(elems + N);
        }

        constexpr const_iterator end() const noexcept {
            return const_iterator(elems + N);
        }

        constexpr const_iterator cend() const noexcept {
            return const_iterator(elems + N);
        }

        /// 以下是反向迭代器
        constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        /// 以下是一个常用接口
        constexpr size_t size() const noexcept {
            return N;
        }

        constexpr size_t max_size() const noexcept {
            return N;
        }

        constexpr bool empty() const noexcept {
            return N == 0;
        }

        /// at()
        constexpr reference at(size_type idx) {
            /// 必须进行边界检查
            if (idx >= N) {
                throw std::out_of_range("array::at");
            }
            return elems[idx];
        }

        constexpr const_reference at(size_type idx) const {
            /// 必须进行边界检查
            if (idx >= N) {
                throw std::out_of_range("array::at");
            }
            return elems[idx];
        }

        /// fill用value填充
        void fill(const T &value) {
            for (size_type i = 0; i < N; i++) {
                elems[i] = value;
            }
        }

        /// swap
        void swap(array &other) {
            for (size_type i = 0; i < N; i++) {
                my::swap(elems[i], other.elems[i]);
            }

        }

        /// 重载==  !=
        bool operator==(const array &other) const {
            for (size_type i = 0; i < N; i++) {
                if (elems[i] != other.elems[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const array &other) const {
            return !(*this == other);
        }


    };
}