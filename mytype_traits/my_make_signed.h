/**
 * @file      my_make_signed.h
 * @brief     [把一个整数类型 T 转换为其对应的 有符号类型]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

/// 泛型编程中，不确定传入的是什么类型的值的时候，直接转换成目标类型的值

#include "my_type_identity.h"

template<typename T>
struct make_signed;

template<>
struct make_signed<char> : type_identity<char> {
};

template<>
struct make_signed<signed char> : type_identity<char> {
};

template<>
struct make_signed<unsigned char> : type_identity<char> {
};

template<>
struct make_signed<short> : type_identity<short> {
};

template<>
struct make_signed<unsigned short> : type_identity<short> {
};

template<>
struct make_signed<int> : type_identity<int> {
};

template<>
struct make_signed<unsigned int> : type_identity<int> {
};

template<>
struct make_signed<long> : type_identity<long> {
};

template<>
struct make_signed<unsigned long> : type_identity<long> {
};

template<>
struct make_signed<long long> : type_identity<long long> {
};

template<>
struct make_signed<unsigned long long> : type_identity<long long> {
};

template<typename T>
using make_signed_t = typename make_signed<T>::type;