/**
 * @file      my_is_null_pointer.h
 * @brief     [is_null_pointer]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_remove_cv.h"

template<typename T>
struct is_nullptr_helper : false_type {
};

template<>
struct is_nullptr_helper<decltype(nullptr)> : true_type {
};

template<typename T>
struct is_nullptr : is_nullptr_helper<remove_cv_t<T>> {
};

template<typename T>
inline constexpr bool is_nullptr_v = is_nullptr<T>::value;
