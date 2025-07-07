/**
 * @file      my_forward.h
 * @brief     [万能引用+完美转发]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mytype_traits/my_remove_reference.h"

namespace my {
    /// forward 用于完美转发，保持参数的引用类型
/// 根据传入的模板参数 T，将参数按照它原本的左值/右值类型转发出去，不发生意外的值类别退化
/// 下面是左值的情况  T 可以是 T、T&、T&&  --> t 是 remove_reference_t<T>&，也就是说 t 本身是个左值引用
/// 返回 T&&：即根据模板参数 T 的引用性质返回左值或右值引用
    template<typename T>
    inline constexpr T &&forward(remove_reference_t<T> &t) noexcept {
        return static_cast<T &&>(t);
    }

/// 右值引用的情况
    template<typename T>
    inline constexpr T &&forward(remove_reference_t<T> &&t) noexcept {
        return static_cast<T &&>(t);
    }
}
