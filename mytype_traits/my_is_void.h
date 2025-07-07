/**
 * @file      my_is_void.h
 * @brief     [判断是不是空类型]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_is_same.h"
#include "my_remove_cv.h"

/// 判断之前需要首先移除cv
template<typename T>
struct is_void : is_same<remove_cv_t<T>, void> {
};

template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;

