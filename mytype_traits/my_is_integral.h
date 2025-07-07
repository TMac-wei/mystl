/**
 * @file      my_is_integral.h
 * @brief     [is_integral]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_remove_cv.h"

/// 首先辅助模板判断裸类型是int
template<typename T>
struct is_integral_ : false_type {
};

/// 对去除cv之后的类型，所有标准的整型，
/// 包括 bool、char 系列（含 char16_t、char32_t）、short、int、long、long long，及其无符号版本
template<>
struct is_integral_<bool> : true_type {
};

template<>
struct is_integral_<char> : true_type {
};

template<>
struct is_integral_<signed char> : true_type {
};

template<>
struct is_integral_<unsigned char> : true_type {
};

template<>
struct is_integral_<wchar_t> : true_type {
};

template<>
struct is_integral_<char16_t> : true_type {
};

template<>
struct is_integral_<char32_t> : true_type {
};

template<>
struct is_integral_<short> : true_type {
};

template<>
struct is_integral_<unsigned short> : true_type {
};

template<>
struct is_integral_<int> : true_type {
};

template<>
struct is_integral_<unsigned int> : true_type {
};

template<>
struct is_integral_<long> : true_type {
};

template<>
struct is_integral_<unsigned long> : true_type {
};

template<>
struct is_integral_<long long> : true_type {
};

template<>
struct is_integral_<unsigned long long> : true_type {
};

/// 外部调用辅助类型模板首先去除cv拿到裸类型之后，继承得到的结果
template<typename T>
struct is_integral : is_integral_<remove_cv_t<T>> {
};

template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;