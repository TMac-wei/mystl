/**
 * @file      my_declval.h
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mytype_traits/my_integral_constant.h"
#include "mytype_traits/my_add_rvalue_reference.h"

namespace my {
    template <typename T>
    add_rvalue_reference_t<T> declval() {
        /// 确保只在decltype中使用declval,不在实际求值的上下文中使用
        static_assert(false_type::value, "declval<T>() called");
    }
}

