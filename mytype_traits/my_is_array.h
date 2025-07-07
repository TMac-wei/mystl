/**
 * @file      my_is_array.h
 * @brief     [判断是否是array]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <cstddef>
#include "my_integral_constant.h"

/// 默认非array
template<typename T>
struct is_array : false_type {
};

/// array 配不定长数组类型，例如：int[]、double[]
template<typename T>
struct is_array<T[]> : true_type {
};

/// 已知大小的数组array
template<typename T, size_t N>
struct is_array<T[N]> : true_type {
};

/// 辅助变量模板
template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;
