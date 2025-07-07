/**
 * @file      my_optional.h
 * @brief     [optional的复现，包含主要的成员接口]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "my_utility.h"
#include "my_exception.h"
#include "my_type_traits.h"

namespace my {
    /// 首先顶一个空值标记类型，用于表示optional不包含值的情况
    struct nullopt_t {
        /// 内部私有结构体，主要用于构建Nullopt_t实例
        struct init {
        };

        /// 私有构造函数，只能通过构造一个init类型的临时对象来实现
        constexpr explicit nullopt_t(init) {}
    };

    /// 全局nullopt_t对象，用于初始化或赋值给optional表示不包含值
    constexpr nullopt_t nullopt{nullopt_t::init()};

    /// 模板类optional，可以存储或者不存储类型T的值
    template<typename T>
    class optional {
    public:
        /// 默认构造函数
        optional() : has_value_(false) {}

        /// 使用nullopt_t对象作为参数构造函数，创建一个不包含值的optional
        optional(nullopt_t) : has_value_(false) {}

        /// 使用类型T的const引用作为参数构造函数，创建一个包含值的optional
        optional(const T &value) : has_value_(true), value_(value) {}

        /// 使用类型T的const右值引用作为参数构造函数，创建一个包含值的optional，并且移动构造值
        optional(T &&value) : has_value_(true), value_(my::move(value)) {}

        /// 模板构造函数，允许从可转换为T的类型U构造optional，需要类型特征检查
        template<typename U, typename = typename my::enable_if_t<my::is_convertible_v<U, T>>>
        optional(U &&value) : has_value_(true), value_(my::forward<U>(value)) {}

        /// 拷贝构造函数，应该首先检查has_value_，根据has_value_判断是否需要进行赋值
        optional(const optional &other) : has_value_(other.has_value_) {
            if (has_value_) {
                value_ = other.value_;
            }
        }

        /// 移动构造函数
        optional(optional &&other) noexcept
                : has_value_(other.has_value_) {
            if (has_value_) {
                value_ = my::move(other.value_);
            }
        }

        /// 模板函数，允许可转换为类型T的类型U赋值给optional，需要进行类型特征检查
        template<typename U, typename = typename my::enable_if_t<my::is_convertible_v<U, T>>>
        optional &operator=(const U &value) {
            has_value_ = true;
            value_ = value;
            return *this;
        }

        /// 拷贝赋值运算符
        optional &operator=(const optional &other) {
            if (this != &other) {
                /// 如果other.has_value_ = true才能进行赋值
                if (other.has_value_) {
                    if (has_value_) {
                        /// 此时自己已经有值，即has_value_ = true,直接赋值
                        value_ = other.value_;
                    } else {
                        /// 自己还没有值，has_value_ = false，赋值并且修改has_value_状态
                        has_value_ = true;
                        value_ = other.value_;  /// 构造新值
                    }
                } else {
                    /// 如果Other没有值，则清空自身
                    has_value_ = false;
                }
            }
            return *this;
        }

        /// 移动赋值运算符
        optional &operator=(optional &&other) noexcept {
            if (this != &other) {
                if (other.has_value_) {
                    if (has_value_) {
                        value_ = my::move(other.value_);
                    } else {
                        value_ = my::move(other.value_);
                        has_value_ = true;
                    }
                } else {
                    has_value_ = false;
                }
            }
            return *this;
        }

        /// 使用nullopt_t对象作为参数赋值运算符，将optional设置为不包含值
        optional &operator=(nullopt_t) {
            has_value_ = false;
            return *this;
        }

        /// 使用T的左值引用作为参数进行赋值
        optional &operator=(const T &value) {
            has_value_ = true;
            value_ = value;
            return *this;
        }

        /// 使用类型T的右值引用作为参数赋值运算符
        optional &operator=(T &&value) {
            has_value_ = true;
            value_ = my::move(value);
            return *this;
        }

        ~optional() {
            if (has_value_) {
                value_.~T();
            }
        }

        /// 接口函数
        bool has_value() const {
            return has_value_;
        }

        /// 返回值的引用，如果不包含，则抛出异常
        T &value() {
            if (!has_value_) {
                throw my::exception("optional has no value");
            }
            return value_;
        }

        const T &value() const {
            if (!has_value_) {
                throw my::exception("optional has no value");
            }
            return value_;
        }

        /// 如果optional包含值，返回值的副本，否则返回提供的默认值
        T value_or(const T &default_value) const {
            return has_value_ ? value_ : default_value;
        }

        /// 返回值的引用
        T &operator()() {
            return value();
        }

        /// 返回值的const引用
        const T &operator()() const {
            return value();
        }

        /// 交换两个optional的内容
        /// 如果 this 没有值但 other 有值，value_ 未初始化就被 swap，会调用未定义行为！
        optional &swap(optional &other) {
            if (has_value_ && other.has_value_) {
                my::swap(value_, other.value_);
            } else if (has_value_ && !other.has_value_) {
                other = my::move(*this);
                reset();
            } else if (!has_value_ && other.has_value_) {
                *this = my::move(other);
                other.reset();
            }
        }

        /// 重置窗口
        void reset() {
            if (has_value_) {
                value_.~T();
                has_value_ = false;
            }
        }

        explicit operator bool() const noexcept {
            return has_value_;
        }



        /// 返回值的指针（如果不包含则抛出异常）
        T *operator->() {
            return &value();
        }

        /// 返回值的const指针（如果不包含则抛出异常）
        const T *operator->() const {
            return &value();
        }

        /// 返回值的引用
        T &operator*() {
            return value();
        }

        /// 返回值的const引用
        const T &operator*() const {
            return value();
        }

    private:
        bool has_value_;    /// 标记optional是否包含值
        T value_;           /// 存储的值(必须在has_value_为true的情况）
    };

    /// make_optional
    template<typename T>
    optional<T> make_optional(const T &value) {
        return optional<T>(value);
    }

}
