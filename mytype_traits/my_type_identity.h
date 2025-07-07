/**
 * @file      my_type_identity.h
 * @brief     类型包装器
 * @author    Weijh
 * @date      2025/7/2
 * @version   1.0
 */

#pragma once

/// 延迟解析类型并用于SFINAE，配合decltype()安全返回类型
template<typename T>
struct type_identity {
    using type = T;
};
