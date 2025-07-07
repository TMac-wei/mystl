/**
 * @file      my_is_convertible.h
 * @brief     [是否可以类型转换(隐式转换)]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "utility_/my_declval.h"
#include "my_integral_constant.h"
#include "my_conditional.h"
#include "my_is_function.h"
#include "my_is_array.h"
#include "my_is_void.h"

/// 特殊情况处理（SFINAE shortcut）
/// 如果 _From 是 void，或 _To 是函数、数组类型（这三种类型不能通过普通表达式隐式转换）：
/// 直接退化为 is_void<_To>。
/// 即如果 _To 也是 void，转换成立；否则不成立。

/**
template<typename From_, typename To_, bool = is_void_v<From_>
                                              || is_function_v<To_> || is_array_v<To_>>
struct is_convertible_helper {
    using type = typename is_void<To_>::type;
};

/// 正常类型，继续判断
template<typename From_, typename To_>
class is_convertible_helper<From_, To_, false> {
    template<typename To1_>
    static void test_aux_(To1_) noexcept;

    template<typename From1_, typename To1_, typename = decltype(test_aux_<To1_>(declval<From_>()))>
    static true_type test_(int);

    template<typename, typename>
    static false_type test_(...);

public:
    typedef decltype(test_<From_, To_>(0)) type;
};

template<typename From_, typename To_>
struct is_convertible : public is_convertible_helper<From_, To_>::type {
};

template<typename From_, typename To_>
inline constexpr bool is_convertible_v = is_convertible<From_, To_>::value;
*/

/// 上述是参考库中的，感觉些许繁琐

namespace my {
/// 前置，用于表达式SFINAE测试
    template<typename To_>
    void test_aux(To_) noexcept;

/// 普通类型转换检查
    template<typename From, typename To, typename = void>
    struct is_convertible_impl : false_type {
    };

/// SFINAE 成功时（即 From 能隐式转换为 To）
    template<typename From, typename To>
    struct is_convertible_impl<From, To, decltype(void(test_aux<To>(declval<From>())))>
            : true_type {
    };

/// 总控制器：处理 void/function/array 特殊情况
    template<typename From, typename To>
    struct is_convertible
            : conditional_t<
                    is_void_v<From> || is_function_v<To> || is_array_v<To>,
                    is_void<To>,         /// 如果 To 是 void 则转换成立
                    is_convertible_impl<From, To>  /// 否则进入正常检查
            > {
    };

    template<typename From_, typename To_>
    inline constexpr bool is_convertible_v = is_convertible<From_, To_>::value;


}
