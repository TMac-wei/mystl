/**
 * @file      my_add_rvalue_reference.h
 * @brief     为输入的类型添加右值引用
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_type_identity.h"

/// 如果能右值引用就返回添加了右值引用的类型
template<typename T>
auto try_add_rvalue_reference(int) -> type_identity<T &&>;

/// 如果不能右值引用，则返回原类型
template<typename T>
auto try_add_rvalue_reference(...) -> type_identity<T>;

/// 继承try_add_rvalue_reference返回的结果
template<typename T>
struct add_rvalue_reference : decltype(try_add_rvalue_reference<T>(0)) {
};

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
