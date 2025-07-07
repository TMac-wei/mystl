/**
 * @file      my_decay.h
 * @brief     [移除指针、引用、数组修饰符]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_remove_reference.h"
#include "my_conditional.h"
#include "my_is_array.h"
#include "my_is_function.h"
#include "my_add_pointer.h"
#include "my_remove_extent.h"

namespace my {
    template<typename T>
    struct decay {
    private:
        using U = remove_reference_t<T>;

    public:
        using type = conditional_t<is_array_v<U>,
                add_pointer_t<remove_extent_t<U>>,
                conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>>;
    };

    template<typename T>
    using decay_t = typename decay<T>::type;
}

