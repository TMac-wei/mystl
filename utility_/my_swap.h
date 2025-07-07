/**
 * @file      my_swap.h
 * @brief     [swap]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

namespace my {
    template<typename T>
    void swap(T &a, T &b) {
        T temp(a);
        a = b;
        b = temp;
    }
}

