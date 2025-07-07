/**
 * @file      my_is_const.h
 * @brief     判断类型是否为const
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"

/// 默认认为 T 不是 const 类型
template <typename T>
struct is_const : false_type{};
/// / 偏特化：匹配 const T，说明是 const 类型
template <typename T>
struct is_const<const T> : true_type {};

/// 最外层的 const，不会递归分析指针指向的内容是否是 const
///  inline 是为了避免多个翻译单元中出现重复定义的符号，以便让变量模板 is_const_v 能被安全地定义在头文件中并被多个 .cpp 文件包含使用
template <typename T>
inline constexpr bool is_const_v = is_const<T>::value;

