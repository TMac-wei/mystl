/**
 * @file      my_is_same.h
 * @brief     [用于判断两个类型是否一致]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

template<typename T, typename U>
struct is_same : false_type {
};

template<typename T>
struct is_same<T, T> : true_type {
};

template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

