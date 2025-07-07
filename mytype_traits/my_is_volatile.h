/**
 * @file      my_is_volatile.h
 * @brief     [类型是否是volatile]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

template <typename T>
struct is_volatile : false_type {};

template <typename T>
struct is_volatile<volatile T> : true_type {};

template <typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;