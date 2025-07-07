/**
 * @file      my_is_union.h
 * @brief     [判断是否是union类型]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

/// 使用编译器的__is_union(T)判断
template <typename T>
struct is_union : bool_constant<__is_union(T)>{};

template <typename T>
inline constexpr bool is_union_v = is_union<T>::value;
