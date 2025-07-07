/**
 * @file      my_has_virtual_destructor.h
 * @brief     [判断是否具有虚析构函数]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

/// 直接使用编译器提供的 __has_virtual_destructor
template <typename T>
struct has_virtual_destructor : integral_constant<bool, __has_virtual_destructor(T)>{};

template <typename T>
inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;