/**
 * @file      my_conditional.h
 * @brief     [类型选择器]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_type_identity.h"

namespace my {
    /// 根据bool选择类型T还是F
    /// 默认为true选择T
    template<bool B, typename T, typename F>
    struct conditional : type_identity<T> {
    };

    /// 为false选择F
    template<typename T, typename F>
    struct conditional<false, T, F> : type_identity<F> {
    };

    template<bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;
}
