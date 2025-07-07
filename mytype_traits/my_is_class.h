/**
 * @file      my_is_class.h
 * @brief     [判断是否是类类型 class 或 struct，排除 union]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_integral_constant.h"
#include "my_is_union.h"

/// 函数模板声明，参数是类型 int T::*，即“指向 T 类型中 int 成员的指针”
/// 只有当 T 是类类型或 struct 类型时，int T::* 是合法的类型（因为只有类、struct 有成员指针）
/// 返回类型是 bool_constant<!is_union_v<T>>，这里结合了 is_union_v<T> 判断，排除了 union 类型。
/// 也就是说，如果 T 是类（class/struct），且不是 union，则该函数模板匹配成功，返回 true_type
template <typename T>
bool_constant<!is_union_v<T>> class_test_(int T::*);

/// 可变参数模板函数，所有情况都能匹配（但是优先级比上面那个低）
template <typename T>
false_type  class_test_(...);

/// decltype(class_test_<T>(nullptr)) 表达式：
///     调用 class_test_，传入 nullptr，根据重载解析选择合适的 class_test_
template <typename T>
struct is_class : decltype(class_test_<T>(nullptr)){};

template <typename T>
inline constexpr bool is_class_v = is_class<T>::value;
