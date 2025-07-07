/**
 * @file      my_remove_pointer.h
 * @brief     [移除指针]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

template<typename T>
struct remove_pointer {
    using type = T;
};

template<typename T>
struct remove_pointer<T *> {
    using type = T;
};

template<typename T>
struct remove_pointer<T *const> {
    using type = T;
};

template<typename T>
struct remove_pointer<T *volatile> {
    using type = T;
};

template<typename T>
struct remove_pointer<T *const volatile> {
    using type = T;
};

template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;