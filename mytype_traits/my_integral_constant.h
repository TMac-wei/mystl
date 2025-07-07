/**
 * @file      my_integral_constant.h
 * @brief      类型萃取（type traits）
 * @author    Weijh
 * @version   1.0
 */

#pragma once

///封装一个静态常量值 v，类型为 T
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;       /// 便于其他模板提取其值的类型
};

/// 定义布尔常量别名,模板逻辑中作为标签使用
using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

/// 通用的模板别名
template<bool B>
using bool_constant = integral_constant<bool, B>;

