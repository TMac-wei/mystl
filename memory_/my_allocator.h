/**
 * @file      my_allocator.h
 * @brief     [分配器]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include <cstdio>
#include <iostream>

template<typename T>
class MyAlloc {
public:
    // 定义各个数据类型
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    // 构造函数
    MyAlloc() noexcept = default;

    MyAlloc(const MyAlloc &) noexcept = default;

    template<typename U>
    MyAlloc(const MyAlloc<U> &) noexcept {}

    ~MyAlloc() = default;

    /// 接下来是一些成员函数
    pointer address(reference x) const noexcept {
        /// 返回真实的地址
        return std::addressof(x);
    }

    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }

    /// 分配内存空间
    pointer allocate(size_type n) {
//        std::cout << "[MyAlloc] Allocating " << n << " element(s)\n";
        /// 调用全局operator new操作符
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    pointer allocate(size_type n, const void *) {
//        std::cout << "[MyAlloc] Allocating " << n << " element(s)\n";
        return allocate(n);
    }

    /// 销毁内存空间
    void deallocate(pointer p, size_type n) {
//        std::cout << "[MyAlloc] Deallocating " << n << " element(s)\n";
        ::operator delete(p);
    }

    /**
     * max_size() 返回你最多能用这个 allocator 分配多少个类型为 T 的对象
     * static_cast<size_type>(-1)：取得 size_type 类型的最大值（通常是 std::size_t 最大值）
     * */
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    /// 调用构造函数，利用可变参数模板，在这块内存空间中，构造可变数量的对象
    template<typename U, typename...Args>
    void construct(U *p, Args &&... args) {
        /// 调用全局 placement new，不会分配内存，只负责调用构造函数
        /// (static_cast<void*>(p))：告诉编译器这个地址已经是合法的内存，防止类型转换警告
        /// U(...)：调用目标对象类型 U 的构造函数
        ::new(static_cast<void *>(p))U(std::forward<Args>(args)...);
///        std::cout << "[MyAlloc] Constructing \n";
    }

    void destroy(pointer p) {
///        std::cout << "[MyAlloc] Destroying element\n";
        /// 显式调用析构函数
        p->~T();
    }

    /// 将刚刚构造的可变参数列表的对象释放掉
    template<typename U>
    void destroy(U *p) {
        ///   std::cout << "[MyAlloc] Destroying element\n";
        p->~U();
    }

    /// rebind 模板（使得支持转换为其他类型的 allocator）
    template<typename U>
    struct rebind {
        using other = MyAlloc<U>;
    };
};

/// allocator 的比较必须提供
template<typename T, typename U>
bool operator==(const MyAlloc<T> &, const MyAlloc<U> &) {
    return true;
}

template<typename T, typename U>
bool operator!=(const MyAlloc<T> &, const MyAlloc<U> &) {
    return false;
}

