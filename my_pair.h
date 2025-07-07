/**
 * @file      my_pair.h
 * @brief     [pair复现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_type_traits.h"
#include "my_utility.h"

namespace my {
    template<typename T1, typename T2>
    struct pair {
        using first_type = T1;
        using second_type = T2;

        first_type first_;
        second_type second_;

        pair()
                : first_(), second_() {}

//        pair(first_type first, second_type second)
//                : first_(first), second_(second) {}

        pair(const first_type &first, const second_type &second)
                : first_(first), second_(second) {}

        pair(first_type &&first, second_type &&second)
                : first_(my::move(first)), second_(my::move(second)) {}

        /// 拷贝构造，从其他pair来构造
        template<typename U1, typename U2>
        pair(const pair<U1, U2> &other) : first_(other.first), second_(other.second) {}

        template<typename U1, typename U2>
        pair(pair<U1, U2> &&other) : first_(my::forward<U1>(other.first)), second_(my::forward<U2>(other.second)) {}


        template<typename U1, typename U2>
        pair &operator=(const pair<U1, U2> &other) {
            first_ = other.first_;
            second_ = other.second_;
            return *this;
        }

        template<typename U1, typename U2>
        pair &operator=(pair<U1, U2> &&other) noexcept {
            first_ = my::forward<U1>(other.first_);
            second_ = my::forward<U2>(other.second_);
            return *this;
        }

        /// 交换两个pair的值
        void swap(pair &other) noexcept {
            using my::swap;
            swap(first_, other.first_);
            swap(second_, other.second_);
        }

        bool operator==(const pair &other) const {
            return first_ == other.first_ && second_ == other.second_;
        }

        bool operator!=(const pair &other) const {
            return !(*this == other);
        }

        bool operator<(const pair &other) {
            return (first_ < other.first_) || (!(other.first_ < first_) && second_ < other.second_);
        }
    };

    /// 构造make_pair辅助函数
    template<typename T1, typename T2>
    constexpr pair<decay_t<T1>, decay_t<T2>>
    make_pair(T1 &&first, T2 &&second) {
        return pair<decay_t<T1>, decay_t<T2>>(my::forward<T1>(first), my::forward<T2>(second));
    }
}