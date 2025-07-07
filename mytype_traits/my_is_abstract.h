/**
 * @file      my_is_abstract.h
 * @brief     [是否是抽象类]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

/// 直接调用编译器的__is_abstract
template<typename T>
struct is_abstract : integral_constant<bool, __is_abstract(T)> {
};

template<typename T>
inline constexpr bool is_abstract_v = is_abstract<T>::value;


