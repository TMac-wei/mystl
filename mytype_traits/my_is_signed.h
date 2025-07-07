/**
 * @file      my_is_signed.h
 * @brief     [判断是否是有符号类型]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_is_integral.h"
#include "my_is_arithmetic.h"
#include "my_is_floating_point.h"
#include "my_conditional.h"

/**
/// 库实现方法
/// T是int型--->T(-1) < T(0)  说明能表示负数-->有符号类型
/// int(-1) < int(0) → true → 有符号
/// unsigned(-1) 会变成很大的数，不小于0 → 无符号
template<typename T, bool = is_integral<T>::value>
struct is_signed_helper_impl : bool_constant<(T(-1) < T(0))> {
};

template<typename T>
struct is_signed_helper_impl<T, false> : true_type {
};  /// 浮点数默认有符号

template<typename T, bool = is_arithmetic_v<T>>
struct is_signed_helper_ : is_signed_helper_impl<T> {
};

template<typename T>
struct is_signed_helper_<T, false> : false_type {
};

template<typename T>
struct is_signed : is_signed_helper_<T> {
};

template<typename T>
inline constexpr bool is_signed_v = is_signed<T>::value;
*/

namespace my {
    /// 判断整数是否为整数类型
    template<typename T>
    struct is_signed_int : bool_constant<T(-1) < T(0)> {
    };


    template<typename T>
    struct is_signed_float : true_type {};

/// 主实现：根据类型分类判断
    template <typename T>
    struct is_signed
            : conditional_t<
                    is_integral_v<T>,
                    is_signed_int<T>,
                    conditional_t<
                            is_floating_point_v<T>,
                            is_signed_float<T>,
                            false_type>> {};

    template<typename T>
    inline constexpr bool is_signed_v = is_signed<T>::value;
}
