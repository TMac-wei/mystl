/**
 * @file      my_is_bounded_array.h
 * @brief     [判断给定类型 T 是否是边界未知的数组类型（unbounded array）]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

template<typename T>
struct is_bounded_array : false_type {
};

template<typename T>
struct is_bounded_array<T[]> : true_type {
};

template<typename T>
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

