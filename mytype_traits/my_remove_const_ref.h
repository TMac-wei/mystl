/**
 * @file      my_remove_const_ref.h
 * @brief     [移除const&]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_remove_const.h"
#include "my_remove_reference.h"

template<typename T>
struct remove_const_ref {
    /// 先移除引用，再移除const
    using type = remove_const_t<remove_reference_t<T>>;
};

template<typename T>
using remove_const_ref_t = typename remove_const_ref<T>::type;
