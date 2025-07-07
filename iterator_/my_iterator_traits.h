/**
 * @file      my_iterator_traits.h
 * @brief     [迭代器机制的简单复刻]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

/**
 * 这个程序是对c++ stl迭代器机制的简单复刻，包括
 * 1、6种迭代器标签
 * 2、通用iterator模板
 * 3、iterator_traits提取机制
 * 4、辅助别名模板
 * */

#include <cstddef>

template<class Iter_>
struct iterator_traits;

/// 常规的6种迭代器，功能上逐渐增强,
/// 类型标签（tag dispatch），用于在算法中通过类型萃取来确定迭代器功能,
/// 这些标签不是用来继承行为，而是用于编译期类型推导（tag dispatching）,以此实现不同的策略
/// 1、输入迭代器，主要操作++（前进到下一个元素），*（解引用以访问当前元素），只读，只能读取一次
struct input_iterator_tag {
};

/// 2、输出迭代器，主要操作++（前进到下一个元素），*，只写，只能写入一次
struct output_iterator_tag {
};

/// 3、前向迭代器，主要操作++、*，可以多次读取和写入相同的元素
struct forward_iterator_tag : public input_iterator_tag {
};

/// 4、双向迭代器，主要操作++、--、*，支持双向移动
struct bidirectional_iterator_tag : public forward_iterator_tag {
};

/// 5、 随机访问迭代器，支持操作++、--、*、+、[]、-（两个迭代器相减）
struct random_access_iterator_tag : public bidirectional_iterator_tag {
};

/// 6、连续迭代器，支持随机访问迭代器的所有操作，但是在连续内存中, C++20
struct contiguous_iterator_tag : public random_access_iterator_tag {
};

/// 以下是具体迭代器的实现，可以继承这个模板来自动获得类型定义
/// 这个类本身没有任何接口（如 operator*、operator++），仅仅是类型定义辅助
template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T *, typename Reference = T &>
struct iterator {
    using iterator_category = Category;     /// 迭代器类型
    using value_type = T;                   /// 迭代器指向的对象类型
    using difference_type = Distance;       ///  两个迭代器之间的距离
    using pointer = Pointer;             /// 指向迭代器指向的对象的指针
    using reference = Reference;         /// 迭代器指向的对象的引用
};

/// traits 通过迭代器获取迭代器的类型，根据迭代器类型提取其相关 typedef 信息
template<typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

/// 针对原生指针的特化
template<typename T>
struct iterator_traits<T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using reference = T &;
};

/// traits 通过 const 指针获取迭代器的类型
template<typename T>
struct iterator_traits<const T *> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;
};

/// 为什么=后面要加上typename？因为 iterator_traits<Iterator>::iterator_category
/// 是一个依赖于模板参数的类型，编译器无法确定 iterator_traits<Iterator>::iterator_category
/// 是一个类型还是一个成员变量，所以需要加上 typename 告诉编译器 iterator_traits<Iterator>::iterator_category 是一个类型

template<typename Iterator>
using iterator_catorgory_t = typename iterator_traits<Iterator>::iterator_category;

template<typename Iterator>
using iterator_value_t = typename iterator_traits<Iterator>::value_type;

template<typename Iterator>
using iterator_difference_t = typename iterator_traits<Iterator>::difference_type;

template<typename Iterator>
using iterator_pointer_t = typename iterator_traits<Iterator>::pointer;

template<typename Iterator>
using iterator_reference_t = typename iterator_traits<Iterator>::reference;