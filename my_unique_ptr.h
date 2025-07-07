/**
 * @file      my_unique_ptr.h
 * @brief     [unique_ptr]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 以下是unique_ptr的复现代码
#include "my_utility.h"
#include <cstddef>
#include <algorithm>

namespace my {
    /**
     * C++标准库专门使用operator()来封装删除行为
     * 原因：希望default_deleter可以像一个可调用对象（仿函数），可以像函数调用一样使用
     * */
    /// 提供2个默认删除器，一个可以针对单个对象进行删除，一个可以针对数组进行删除
    /// 策略模式 + 函数对象（函数仿函数）
    template<typename Tp>
    struct default_deleter {
        constexpr default_deleter() noexcept = default;

        void operator()(Tp *ptr) const {
            delete ptr;
        }
    };

    /// 特化版本：针对数组进行删除
    template<typename Tp>
    struct default_deleter<Tp[]> {
        constexpr default_deleter() noexcept = default;

        void operator()(Tp *ptr) const {
            delete[]ptr;
        }
    };


    /// 针对单个对象
    template<typename Tp, typename Dp = default_deleter<Tp>>
    class unique_ptr {
    public:
        /// 类型定义
        using pointer = Tp *;
        using element_type = Tp;
        using delete_type = Dp;
    public:
        constexpr unique_ptr() noexcept
                : ptr_(nullptr) {}

        explicit unique_ptr(pointer ptr) noexcept
                : ptr_(ptr) {}

        /// 这里的参数是std::nullptr_t表明允许接收一个nullptr类型的参数，目的是可以显式构造一个空的unique_ptr
        /// 这非常有用，比如作为默认初始化
        explicit unique_ptr(std::nullptr_t) noexcept
                : ptr_(nullptr) {}

        /// 拷贝构造和移动构造
        unique_ptr(pointer ptr, const delete_type &deleter) noexcept
                : ptr_(ptr), deleter_(deleter) {}

        unique_ptr(pointer ptr, delete_type &&deleter) noexcept
                : ptr_(ptr), deleter_(my::move(deleter)) {}

        /// unique_ptr的核心是不允许通过unique_ptr进行拷贝和赋值
        unique_ptr(const unique_ptr &other) = delete;

        unique_ptr &operator=(const unique_ptr &other) = delete;

        /// 模板移动构造函数和模板移动赋值运算符
        /// 允许从其它类型的 unique_ptr<U> 构造或赋值，只要 U* 可以转换为 T*
        template<typename U>
        unique_ptr(unique_ptr<U> &&other) noexcept
                : ptr_(nullptr) {
            static_assert(std::is_convertible<U *, Tp *>::value,
                          "U* must be convertible to Tp*");

            /// 使用swap是一种安全移动资源的方法，可以自动交换内部指针；原指针置空，不会double free，
            /// 即使类型不同（只要U*能够转换为T*）就能兼容
            other.swap(*this);
        }

        template<typename U>
        unique_ptr &operator=(unique_ptr<U> &&other) noexcept {
            other.swap(*this);
            return *this;
        }

        /// 重载operator=接收nullptr_t
        unique_ptr &operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        ~unique_ptr() {
            reset();
        }

        /// reset 重置当前指针所持资源，先释放原资源（如果有），然后设置为新资源
        void reset(pointer ptr = pointer()) noexcept {
            /// 重置指针，删除当前管理的对象
            /// 需要做自删除判断
            if (ptr_ != ptr) {
                pointer temp = ptr_;
                ptr_ = ptr;
                if (temp) {
                    deleter_(temp);
                }
            }
        }

        pointer release() noexcept {
            /// 释放控制权但不释放资源，这里指的是将Ptr的所有权交出，不会调用删除器，返回原始指针，由程序自己管理资源
            pointer temp = ptr_;            /// 拷贝当前持有的指针到 temp
            ptr_ = nullptr;                 /// 将内部指针ptr_设置为nullptr(防止double free)
            return temp;                    /// 返回原始指针
        }

        /// 获取指针
        pointer get() const noexcept {
            return ptr_;
        }

        /// 获取删除器
        delete_type getdeleter() noexcept {
            return deleter_;
        }

        const delete_type getdeleter() const noexcept {
            return deleter_;
        }

        explicit operator bool() const noexcept {
            return ptr_ != nullptr;
        }

        pointer operator->() const noexcept {
            return get();
        }

        /// 解引用
        Tp &operator*() const noexcept {
            return *ptr_;
        }

        /// swap交换资源
        void swap(unique_ptr &other) noexcept {
            my::swap(ptr_, other.ptr_);
            my::swap(deleter_, other.deleter_);
        }


    private:
        /// 成员变量主要有两个 1、变量指针 2、删除器
        pointer ptr_;
        delete_type deleter_;
    };

    /// 针对对象数组
    template<typename Tp, typename Dp>
    class unique_ptr<Tp[], Dp> {
    public:
        using pointer = Tp *;
        using element_type = Tp;
        using deleter_type = Dp;
    public:
        unique_ptr() noexcept
                : ptr_(nullptr) {}

        unique_ptr(pointer ptr) noexcept
                : ptr_(ptr) {}

        unique_ptr(std::nullptr_t) noexcept
                : ptr_(nullptr) {}

        unique_ptr(pointer ptr, const deleter_type &deleter) noexcept
                : ptr_(ptr), deleter_(deleter) {}

        unique_ptr(pointer ptr, deleter_type &&deleter) noexcept
                : ptr_(ptr), deleter_(my::move(deleter)) {}

        /// 核心是不允许拷贝构造和赋值
        unique_ptr(const unique_ptr &) = delete;

        unique_ptr &operator=(const unique_ptr &) = delete;

        /// 移动构造和移动赋值运算符
        template<typename U>
        unique_ptr(unique_ptr<U[]> &&other) noexcept
                : ptr_(nullptr) {
            static_assert(std::is_convertible<U *, Tp *>::value,
                          "U* must be convertible to Tp*");

            other.swap(*this);
        }

        template<typename U>
        unique_ptr &operator=(unique_ptr<U[]> &&other) noexcept {
            other.swap(*this);
            return *this;
        }

        /// 重载operator=接收nullptr_t
        unique_ptr &operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        ~unique_ptr() {
            /// 重置当前的指针
            reset();
        }

        void reset(pointer ptr = pointer()) noexcept {
            /// 需要做自删除判断
            if (ptr_ != ptr) {
                pointer temp = ptr_;
                ptr_ = ptr;
                if (temp) {
                    deleter_(temp);
                }
            }
        }

        pointer release() noexcept {
            pointer tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        }

        /// get获取当前指针
        pointer get() const noexcept {
            return ptr_;
        }

        explicit operator bool() const noexcept {
            return ptr_ != nullptr;
        }

        Tp &operator[](size_t idx) const noexcept {
            /// 在标准库中[]是没有做越界判断的，一般需要自己在外部调用时进行边界检测
            return ptr_[idx];
        }

        void swap(unique_ptr &other) noexcept {
            my::swap(ptr_, other.ptr_);
            my::swap(deleter_, other.deleter_);
        }

    private:
        pointer ptr_;
        deleter_type deleter_;
    };

}

