/**
 * @file      my_enable_if.h
 * @brief     enable_if
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */


#pragma once

namespace my {
    /// 通用默认模板
    template<bool, typename T = void>
    struct enable_if {
    };

/// 特化版本
    template<typename T>
    struct enable_if<true, T> {
        using type = T;
    };

/// 创建别名
    template<bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;
}
