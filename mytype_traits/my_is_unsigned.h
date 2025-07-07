/**
 * @file      my_is_unsigned.h
 * @brief     [判断无符号类型]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_is_integral.h"
#include "my_integral_constant.h"
#include "my_is_arithmetic.h"

template<typename T, bool = is_integral_v<T>>
struct is_unsigned_helper_impl : integral_constant<bool, T(0) < T(-1)> {
};   /// T(0) < T(-1)为真表示无符号类型

template<typename T>
struct is_unsigned_helper_impl<T, false> : false_type {
};

template<typename T, bool = is_arithmetic_v<T>>
struct is_unsigned_helper_ : is_unsigned_helper_impl<T> {
};

template<typename T>
struct is_unsigned_helper_<T, false> : false_type {
};

template<typename T>
struct is_unsigned : is_unsigned_helper_<T> {
};

template<typename T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
