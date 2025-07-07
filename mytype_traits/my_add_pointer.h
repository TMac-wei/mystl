/**
 * @file      my_add_pointer.h
 * @brief     类型萃取工具 add_pointer<T>:将类型 T 转换为指针类型 T*
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */

#pragma once

#include "my_type_identity.h"
#include "my_remove_reference.h"

/**
 * 将类型 T 转换为指针类型 T*，除非不合法（比如不能对函数引用类型加 * 时返回原类型）
 * 实现效果：
        add_pointer_t<int>      → int*
        add_pointer_t<int&>     → int*
        add_pointer_t<void()>   → void(*)()
        add_pointer_t<int[3]>   → int(*)[3]
        add_pointer_t<void>     → void*
 * */

/// 辅助函数模板，SFINAE检测能否添加指针
/// 需要先移除引用再加*
template <typename T>
auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*>;

/// 如果不能添加指针，则返回原类型
template <typename T>
auto try_add_pointer(...) ->type_identity<T>;

/// 使用参数为 int 的函数调用确保优先匹配第一重载,继承decltype从辅助函数获取的返回值
template <typename T>
struct add_pointer : decltype(try_add_pointer<T>(0)){};
/// 等价于
//type_identity<X>  -->  struct add_pointer{using type = X;}

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;