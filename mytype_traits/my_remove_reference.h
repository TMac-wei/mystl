/**
 * @file      my_remove_reference.h
 * @brief      移除类型 T 的引用修饰符，（无论是左值引用 T& 还是右值引用 T&&），返回裸类型 T
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */

#pragma once

/// 默认情况
template<typename T>
struct remove_reference {
    using type = T;
};

/// 去除左值引用
template<typename T>
struct remove_reference<T &> {
    using type = T;
};

/// 去除右值引用
template<typename T>
struct remove_reference<T &&> {
    using type = T;
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;