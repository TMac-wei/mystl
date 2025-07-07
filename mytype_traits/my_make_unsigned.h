/**
 * @file      my_make_unsigned.h
 * @brief     [	把一个整数类型 T 转换为其对应的 无符号类型]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_type_identity.h"

template<typename T>
struct make_unsigned;

template<>
struct make_unsigned<char> : type_identity<unsigned char> {
};

template<>
struct make_unsigned<signed char> : type_identity<unsigned char> {
};

template<>
struct make_unsigned<short> : type_identity<unsigned short> {
};

template<>
struct make_unsigned<int> : type_identity<unsigned int> {
};

template<>
struct make_unsigned<long> : type_identity<unsigned long> {
};

template<>
struct make_unsigned<long long> : type_identity<unsigned long long> {
};

template<>
struct make_unsigned<unsigned char> : type_identity<unsigned char> {
};

template<>
struct make_unsigned<unsigned short> : type_identity<unsigned short> {
};

template<>
struct make_unsigned<unsigned int> : type_identity<unsigned int> {
};

template<>
struct make_unsigned<unsigned long> : type_identity<unsigned long> {
};

template<>
struct make_unsigned<unsigned long long> : type_identity<unsigned long long> {
};

template<typename T>
using make_unsigned_t = typename make_unsigned<T>::type;