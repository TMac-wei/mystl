/**
 * @file      my_is_unbounded_array.h
 * @brief     [判断无边界的数组]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

template<typename T>
struct is_unbounded_array : false_type {
};

template<typename T>
struct is_unbounded_array<T[]> : true_type {
};

template<typename T>
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

