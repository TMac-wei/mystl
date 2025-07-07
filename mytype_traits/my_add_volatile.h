/**
 * @file      my_add_volatile.h
 * @brief     为类型添加volatile
 * @author    Weijh
 * @version   1.0
 */

#pragma once

template <typename T>
struct add_volatile {
    using type = volatile T;
};

template <typename T>
using add_volatile_t = typename add_volatile<T>::type;
