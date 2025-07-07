/**
 * @file      my_is_function.h
 * @brief     [is_function ]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_is_const.h"
#include "my_is_reference.h"
#include "my_integral_constant.h"
#include "my_remove_cv.h"

/// is_function_helper 做偏特化的匹配分发
template<typename>
struct is_function_helper : false_type {
};

///  特化版本 普通函数 (Ret(Args...)) 接受零个或多个参数，但这些参数必须明确指定类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...)> : true_type {
};

///  特化版本 可变参数函数 (Ret(Args......))  C风格可变参数函数类型（variadic function）
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...)> : true_type {
};

///  特化版本  const、volatile修饰符
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile> : true_type {
};

/// C风格的可变参函数类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile> : true_type {
};

///  特化版本  & 修饰符
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile &> : true_type {
};

/// C风格的可变参函数类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile &> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile &> : true_type {
};

///  特化版本  && 修饰符
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile &&> : true_type {
};

/// C风格的可变参函数类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile &&> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile &&> : true_type {
};

///  特化版本 正常函数 noexcept
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile noexcept> : true_type {
};

///  特化版本 左值引用 noexcept
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile &noexcept> : true_type {
};

/// C风格的可变参函数类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile &noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile &noexcept> : true_type {
};

///  特化版本 右值引用 noexcept
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) volatile &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args...) const volatile &&noexcept> : true_type {
};

/// C风格的可变参函数类型
template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) volatile &&noexcept> : true_type {
};

template<typename Ret, typename ... Args>
struct is_function_helper<Ret(Args..., ...) const volatile &&noexcept> : true_type {
};

template<typename T>
struct is_function : is_function_helper<remove_cv<T>> {
};

template<typename T>
inline constexpr bool is_function_v = is_function<T>::value;