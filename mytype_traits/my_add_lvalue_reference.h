/**
 * @file      my_add_lvalue_reference.h
 * @brief     为输入的类型添加左值引用
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_type_identity.h"

/**
 *
 * 为输入的类型添加左值引用，需要先判断输入类型是否可以添加左值引用
 * 目标效果：
        add_lvalue_reference_t<int>         // int&
        add_lvalue_reference_t<int&>        // int&
        add_lvalue_reference_t<void>        // void
        add_lvalue_reference_t<int(int)>    // int(int)
 * */

/// 类型 T& 是合法类型时，匹配这个模板
template<typename T>
auto try_add_lvalue_reference(int) -> type_identity<T&>;

/// 匹配不成功 T& 时，返回原类型
template<typename T>
auto try_add_lvalue_reference(...) -> type_identity<T>;

/// 尝试匹配int参数的版本
template<typename T>
struct add_lvalue_reference : decltype(try_add_lvalue_reference<T>(0)){};

/// 别名模板
template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
