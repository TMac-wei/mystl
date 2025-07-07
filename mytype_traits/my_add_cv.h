/**
 * @file      my_add_cv.h
 * @brief     添加const volatile修饰符
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 添加cv修饰符 const volatile
template<typename T>
struct add_cv {
    using type = const volatile T;
};

template<typename T>
using add_cv_t = typename add_cv<T>::type;