/**
 * @file      my_is_floating_point.h
 * @brief     [my_is_floating_point]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_remove_cv.h"

template<typename T>
struct is_floating_point_helper_ : false_type {
};

template<>
struct is_floating_point_helper_<float> : true_type {
};

template<>
struct is_floating_point_helper_<double> : true_type {
};

template<>
struct is_floating_point_helper_<long double> : true_type {
};

template<typename T>
struct is_floating_point : is_floating_point_helper_<remove_cv_t<T>> {
};

template<typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
