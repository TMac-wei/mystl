/**
 * @file      my_uninitialized.h
 * @brief     [底层内存管理工具实现]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

/**
 *  是容器底层实现中构造对象的核心操作，特别适用于 vector、deque、list 等
 *  这些函数都实现了 原始内存上构造对象 的能力（placement new），用于：
 *  1、分配但未构造的内存中构造对象
 *  2、异常安全：构造失败时释放已构造的对象
 *  3、支持自定义分配器（Allocator）
 *  模拟标准库中的：
 *      std::uninitialized_fill
 *      std::uninitialized_copy
 *      std::uninitialized_default_construct
 * */

#include "my_utility.h"
#include "my_type_traits.h"
#include "memory_/my_allocator.h"

/// 对 [first, last) 区间的对象调用 destroy
/// 构造过程中抛出异常，需要回滚已经构造的对象
template<typename InputIterator, typename Alloc>
void destroy_(InputIterator first, InputIterator last, Alloc &alloc) {
    for (; first != last; ++first) {
//        alloc.destroy(std::addressof(*first));
        std::allocator_traits<Alloc>::destroy(alloc, std::addressof(*first));
    }

}

/// 在 [first, first + n) 范围内，用 value 拷贝构造 n 个对象（未初始化空间），使用自定义分配器 alloc -> 返回最后一个构造后的位置（cur）
/// 如果构造失败，则析构刚刚创建过的对象，并且释放内存、抛出异常
template<typename ForwardIterator, typename Size, typename T, typename Alloc>
ForwardIterator uninitialized_fill_n_a(ForwardIterator first, Size n, const T &value, Alloc &alloc) {
    ForwardIterator cur = first;  /// 记录当前构造进度位置，用于异常时回滚
    try {
        for (;n > 0; --n, ++cur) {
            /// 调用分配构造函数，用指定值构造对象
            alloc.construct(std::addressof(*cur), value);
        }
        return cur;  /// 返回下一个未初始化的位置
    } catch (...) {
        /// 析构已经成功构造的 [first, cur) 区间
        destroy_(first, cur, alloc);
        throw;
    }
}

/// 将 [first, last) 中每个位置构造成 value 的拷贝
/// 使用场景：用于插入构造固定值的区间，如 vector::insert(pos, count, value)，需要构造 [pos, pos + count)
template<typename ForwardIterator, typename T, typename Alloc>
ForwardIterator uninitialized_fill_a(ForwardIterator first, ForwardIterator last, const T &value, Alloc &alloc) {
    ForwardIterator cur = first;  /// 记录构造进度
    try {
        for (;first != last; ++first) {
            /// 调用分配构造函数，用指定值构造对象
            alloc.construct(std::addressof(*first), value);
        }
    } catch (...) {
        /// 异常发生时，回滚 [cur, first) 区间，即已经成功构造的对象，防止内存泄漏
        destroy_(cur, first, alloc);
        throw;
    }
}

/// 使用默认构造函数构造 n 个对象到 first 开始的内存空间
//// 构造过程中失败，就销毁 [first, cur) 区间--->保证异常安全
template <typename ForwardIterator, typename Size, typename Alloc>
ForwardIterator uninitalized_default_n_a(ForwardIterator first, Size n, Alloc& alloc) {
    ForwardIterator cur = first;    /// 记录构造进度
    try{
        for(; n > 0; --n, ++cur) {
            alloc.construct(std::addressof(*cur));
        }
        return cur;     /// 返回下一个未初始化的位置
    } catch (...) {
//        for (; first != cur; ++first) {
//            alloc.destroy(std::addressof(*first));
//        }
        destroy_(first, cur, alloc);
        throw;
    }
}

/// 在未初始化的内存区间 [first, last) 中，使用默认构造函数构造对象,作用是初始化 [first, last) 区间内的对象
/// 返回值是void--> 因为它不生成新的区间或中间迭代器，它不像 uninitialized_copy / uninitialized_fill_n 需要返回“下一个位置”以支持链式构造
template <typename ForwordIterator, typename Alloc>
void uninitialized_default_a(ForwordIterator first, ForwordIterator last, Alloc& alloc) {
    ForwordIterator cur = first;
    try {
        for (; first != last; ++first) {
//            alloc.construct(std::addressof(*first));
            std::allocator_traits<Alloc>::construct(alloc, std::addressof(*first));

        }
    } catch (...) {
        destroy_(cur, first, alloc);
        throw;
    }
}

/// 将 [first, last) 区间元素拷贝到 result 开始的未初始化区域==>等价于 std::uninitialized_copy
/// 在 vector::insert()、realloc() 时，将旧数据复制到新内存中就会用到这个函数
template <typename InputIterator, typename ForwordIterator, typename Alloc>
ForwordIterator uninitialized_copy_a(InputIterator first, InputIterator last, ForwordIterator result, Alloc& alloc) {
    ForwordIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            alloc.construct(std::addressof(*cur), *first);
        }
        return cur;
    } catch(...) {
        destroy_(result, cur, alloc);
        throw ;
    }
}

/// 添加移动语义
template <typename InputIterator, typename ForwordIterator, typename Alloc>
ForwordIterator uninitialized_move_a(InputIterator first, InputIterator last, ForwordIterator result, Alloc& alloc) {
    ForwordIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            /// 使用move将源对象转移构造到目标对象
//            alloc.construct(std::addressof(*cur), std::move(*first));
            /// 使用自己手写的address和move，但是一般都是推荐使用std:;addressof()
            alloc.construct(std::addressof(*cur), move(*first));
        }
        return cur;
    } catch(...) {
        destroy_(result, cur, alloc);  /// 析构已经构造的部分
        throw;
    }
}
