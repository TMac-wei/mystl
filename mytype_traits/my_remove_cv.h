/**
 * @file      my_remove_cv.h
 * @brief     [移除cv]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "my_remove_const.h"
#include "my_remove_volatile.h"

template<typename T>
struct remove_cv {
    using type = remove_const_t<remove_volatile_t<T>>;
};

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;