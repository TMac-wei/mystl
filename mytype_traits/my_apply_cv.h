/**
 * @file      my_apply_cv.h
 * @brief     [cv-ref 属性复制工具]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_is_const.h"
#include "my_is_volatile.h"


/// 将类型T的const和volatile修饰符（以及是否引用）引用到另一个类型U上
/// 通用模板
template <typename T, bool = is_const_v<T>, bool = is_volatile_v<T>>
struct apply_cv_helper_{
    template<typename U>
    using apply_ = U;
};

/// 特化版本 const U
template <typename T>
struct apply_cv_helper_<T, true, false>{
    template<typename U>
    using apply_ = const U;
};

/// 特化版本 volatile U
template <typename T>
struct apply_cv_helper_<T, false, true>{
    template<typename U>
    using apply_ = volatile U;
};

/// 特化版本 const volatile U
template <typename T>
struct apply_cv_helper_<T, true, true>{
    template<typename U>
    using apply_ = const volatile U;
};

/// 引用类型
template <typename T>
struct apply_cv_helper_<T&, false, false>{
    template<typename U>
    using apply_ =  U&;
};

template <typename T>
struct apply_cv_helper_<T&, true, false>{
    template<typename U>
    using apply_ = const U&;
};

template <typename T>
struct apply_cv_helper_<T&, false, true>{
    template<typename U>
    using apply_ = volatile U&;
};

template <typename T>
struct apply_cv_helper_<T&, true, true>{
    template<typename U>
    using apply_ = const volatile U&;
};

template <typename T, typename U>
using apply_cv_t = typename apply_cv_helper_<T>::template apply_<U>;
