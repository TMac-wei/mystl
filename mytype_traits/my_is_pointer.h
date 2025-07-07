/**
 * @file      my_is_pointer.h
 * @brief     [is_pointer]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_remove_cv.h"

/// 判断是否是指针类型
template<typename T>
struct is_pointer_helper : false_type {
};

template<typename T>
struct is_pointer_helper<T *> : true_type {
};

template<typename T>
struct is_pointer : is_pointer_helper<remove_cv_t<T>> {
};

template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;


