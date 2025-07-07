/**
 * @file      my_remove_volatile.h
 * @brief     [移除了类型的volatile]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 移除了类型的volatile
/// 默认没有volatile
template<typename T>
struct remove_volatile {
    using type = T;
};

template<typename T>
struct remove_volatile<volatile T> {
    using type = T;
};

template<typename T>
using remove_volatile_t = typename remove_volatile<T>::type;