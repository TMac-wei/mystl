/**
 * @file      my_remove_const.h
 * @brief     [remove_const]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

/// 移除了类型的const
/// 默认没有const
template<typename T>
struct remove_const {
    using type = T;
};

template<typename T>
struct remove_const<const T> {
    using type = T;
};

template<typename T>
using remove_const_t = typename remove_const<T>::type;