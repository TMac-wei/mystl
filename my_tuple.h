/**
 * @file      my_tuple.h
 * @brief     [tuple]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "my_type_traits.h"
#include "my_utility.h"

namespace my {
    /// 前向声明
    template<typename... Args>
    class Tuple;

    /// 递归终止的特化版本
    template<>
    class Tuple<> {
    };

    /// Tuple 主体
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> {
    public:
        /// 构造函数接受当前元素和剩余元素，递归构造整个元组
        Tuple(Head head, Tail... tail)
                : head_(head), tail_(Tuple<Tail...>(tail...)) {}

        /// 获取头部元素
        Head &getHead() {
            return head_;
        }

        /// 获取tuple的尾部（除第0个元素之外的所有元素）
        Tuple<Tail...> &getTail() {
            return tail_;
        }

        ~Tuple() = default;

    private:
        Head head_;                 /// 存储当前类型的元素
        Tuple<Tail...> tail_;       /// 存储剩余元素的元组
    };

    /// TupleGetter 主模板声明
    template<size_t Index, typename TupleType>
    struct TupleGetter;

    /// 特化：Index == 0
    template<typename Head, typename... Tail>
    struct TupleGetter<0, Tuple<Head, Tail...>> {
        static Head &get(Tuple<Head, Tail...> &t) {
            return t.getHead();
        }
    };

    /// 递归获取 Index > 0 的元素
    template<size_t Index, typename Head, typename... Tail>
    struct TupleGetter<Index, Tuple<Head, Tail...>> {
        static auto &get(Tuple<Head, Tail...> &t) {
            return TupleGetter<Index - 1, Tuple<Tail...>>::get(t.getTail());
        }
    };

    /// get 接口函数，简化get的调用
    template<size_t Index, typename... Args>
    auto &get(Tuple<Args...> &t) {
        return TupleGetter<Index, Tuple<Args...>>::get(t);
    }

    /// make_tuple 实现
    template<typename... Args>
    auto make_tuple(Args &&... args) {
        return Tuple<typename decay<Args>::type...>(my::forward<Args>(args)...);
    }
}
