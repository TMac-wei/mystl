/**
 * @file      my_add_const.h
 * @brief     添加const修饰符
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */

#pragma once

template<typename T>
struct add_const {
    /// 为传进来的类型，添加const修饰符
    using type = const T;
};

template<typename T>
using add_const_t = typename add_const<T>::type;
