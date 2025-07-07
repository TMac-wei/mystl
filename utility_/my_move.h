/**
 * @file      my_move.h
 * @brief     [移动语义]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mytype_traits/my_remove_reference.h"

namespace my {
    /// 将一个对象强制转换为对应的右值引用，从而启用移动语义（move semantics）
/// 返回值是 remove_reference_t<T> &&  --> 去掉引用之之后得到的真正类型的对应的右值引用
    template<typename T>
    inline constexpr remove_reference_t<T> &&move(T &&t) noexcept {
        return static_cast<remove_reference_t<T> &&>(t);        /// 强制转换为右值引用，以支持移动语义
    }
}

