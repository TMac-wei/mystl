/**
 * @file      my_algorithm.h
 * @brief     [max min]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

namespace my {
    template<typename T>
    inline const T &min(const T &a, const T &b) {
        return a < b ? a : b;
    }

    template<typename T>
    inline const T &max(const T &a, const T &b) {
        return a > b ? a : b;
    }
}

