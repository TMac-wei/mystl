/**
 * @file      my_make_const_lvalue_ref.h
 * @brief     [my_make_const_lvalue_ref]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_remove_reference.h"

template<typename T>
struct make_const_lvalue_ref {
    using type = const remove_reference_t<T> &;      /// 不管是什么类型，先移除引用，然后转换为左值引用
};

template<typename T>
using make_const_lvalue_ref_t = typename make_const_lvalue_ref<T>::type;