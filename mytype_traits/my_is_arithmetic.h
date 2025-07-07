/**
 * @file      my_is_arithmetic.h
 * @brief     [是否数字类型]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_is_floating_point.h"
#include "my_is_integral.h"

template<typename T>
struct is_arithmetic : integral_constant<bool, is_integral_v<T> || is_floating_point_v<T>> {
};

template<typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
