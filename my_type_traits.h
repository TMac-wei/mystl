/**
 * @file      my_type_traits.h
 * @brief     [导入类型萃取的所有头文件]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// type_traits的前提条件，编译器会择优选择最匹配的特化版本
/// SFINEAE(Substitution Failure Is Not An Error)：模板失败不是错误

#include "mytype_traits/my_integral_constant.h"
#include "mytype_traits/my_is_abstract.h"
#include "mytype_traits/my_has_virtual_destructor.h"
#include "mytype_traits/my_is_floating_point.h"
#include "mytype_traits/my_is_integral.h"
#include "mytype_traits/my_is_arithmetic.h"
#include "mytype_traits/my_is_array.h"
#include "mytype_traits/my_is_bounded_array.h"
#include "mytype_traits/my_is_const.h"
#include "mytype_traits/my_enable_if.h"
#include "mytype_traits/my_is_same.h"
#include "mytype_traits/my_is_integral.h"
#include "mytype_traits/my_is_null_pointer.h"
#include "mytype_traits/my_is_pointer.h"
#include "mytype_traits/my_is_reference.h"
#include "mytype_traits/my_is_void.h"
#include "mytype_traits/my_is_signed.h"
#include "mytype_traits/my_is_unbounded_array.h"
#include "mytype_traits/my_is_unsigned.h"
#include "mytype_traits/my_is_volatile.h"
#include "mytype_traits/my_remove_const_ref.h"
#include "mytype_traits/my_remove_const.h"
#include "mytype_traits/my_remove_cv.h"
#include "mytype_traits/my_remove_extent.h"
#include "mytype_traits/my_remove_pointer.h"
#include "mytype_traits/my_remove_reference.h"
#include "mytype_traits/my_remove_volatile.h"
#include "mytype_traits/my_make_const_lvalue_ref.h"
#include "mytype_traits/my_make_signed.h"
#include "mytype_traits/my_make_unsigned.h"
#include "mytype_traits/my_add_const.h"
#include "mytype_traits/my_add_cv.h"
#include "mytype_traits/my_add_volatile.h"
#include "mytype_traits/my_void_t.h"
#include "mytype_traits/my_add_lvalue_reference.h"
#include "mytype_traits/my_add_pointer.h"
#include "mytype_traits/my_add_rvalue_reference.h"
#include "mytype_traits/my_is_function.h"
#include "mytype_traits/my_decay.h"
#include "mytype_traits/my_is_union.h"
#include "mytype_traits/my_is_class.h"
#include "mytype_traits/my_is_convertible.h"
