/**
 * @file      my_heap_sort.h
 * @brief     [堆排序--维护大根堆和小根堆]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_vector.h"
#include "my_list.h"
#include "my_iterator.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <memory>


/// 用堆实现优先级队列
namespace my {
    /**
     * push_heap
     * pop_heap
     * sort_heap
     * make_heap
     * adjust_heap
     *
     * _push_heap是一个模板函数、用于将新元素插入到堆中，并且保持堆的性质
     * 通过比较新的元素与父节点的值，并根据比较结果重新排列元素，维持堆的性质
     *
     * 参数：
     *      first--指向堆的开始迭代器
     *      holeIndex-新元素应该插入的位置的索引
     *      topIndex-堆顶元素的索引，通常为0，表示堆的最顶端
     *      value-要插入堆中的新元素的值
     *      comp-比较函数，用于比较两个元素的大小 如果comp(a, b)返回true，表示a < b;
     *
     *      标准函数模板库中的比较函数如下：
     *      template<typename _Tp>
            struct greater : public binary_function<_Tp, _Tp, bool>
            {
              _GLIBCXX14_CONSTEXPR
              bool
              operator()(const _Tp& __x, const _Tp& __y) const
              { return __x > __y; }
            };

          /// One of the @link comparison_functors comparison functors@endlink.
          template<typename _Tp>
            struct less : public binary_function<_Tp, _Tp, bool>
            {
              _GLIBCXX14_CONSTEXPR
              bool
              operator()(const _Tp& __x, const _Tp& __y) const
              { return __x < __y; }
            };
     *
     * */

    /// 这是一个标准模板
    template<typename RandomIt, typename Distance, typename T, typename Comp>
    void _push_heap(RandomIt first, Distance holeIndex, Distance topIndex, T value, Comp comp) {
        /// 计算父节点的索引
        Distance parent = (holeIndex - 1) / 2;

        /// 循环直到堆的顶部，或者父节点的值大于等于新插入的值，向上移动堆中元素，上浮操作为新元素找到合适的位置
        while (holeIndex > topIndex && comp(*(first + parent), value)) {
            /// 如果父亲节点的值小于新的值，则将父亲节点的值往下移动到当前空位
            *(first + holeIndex) = std::move(*(first + parent));
            /// 更新空洞位置为父节点的位置，继续上浮操作
            holeIndex = parent;
            /// 重新计算父节点的索引位置
            parent = (holeIndex - 1) / 2;
        }

        /// 找到新元素的位置之后，将新元素放入该位置
        *(first + holeIndex) = my::move(value);
    }

    template<typename RandomIt>
    void push_heap(RandomIt first, RandomIt last) {
        /// push_heap做了进一步封装
//        using ValueType = typename RandomIt::value_type;
//        using Distance = typename RandomIt::difference_type;
        /// 类型萃取不适用普通指针，
        using ValueType = typename std::iterator_traits<RandomIt>::value_type;
        using Distance = typename std::iterator_traits<RandomIt>::difference_type;
        my::_push_heap(first, Distance(last - first - 1), Distance(0),
                       my::move(*(last - 1)), std::less<ValueType>());

    }

    template<typename RandomIt, typename Comp>
    void push_heap(RandomIt first, RandomIt last, Comp comp) {
        /// push_heap做了进一步封装
//        using ValueType = typename RandomIt::value_type;
//        using Distance = typename RandomIt::difference_type;
        using ValueType = typename std::iterator_traits<RandomIt>::value_type;
        using Distance = typename std::iterator_traits<RandomIt>::difference_type;
        my::_push_heap(first, Distance(last - first - 1), Distance(0),
                       my::move(*(last - 1)), comp);
    }

    /**
     * _adjust_heap 是一个模板函数，用于在堆中调整元素的位置保持堆的性质
     * 主要用于删除堆顶元素或者是在堆中某个节点的值增大之后重新调整堆的结构
     *
     * 参数：
     *      first-指向堆顶序列开始的迭代器
     *      holeIndex-需要调整的节点的索引，通常是被删除的节点的位置
     *      len-堆中元素的总数
     *      value-被调整节点的新值
     *      comp-比较函数的对象，用于比较两个元素的大小，如果comp(a, b)返回true,则表示a < b
     * */
    template<typename RandomIt, typename Distance, typename T, typename Comp>
    void _adjust_heap(RandomIt first, Distance holeIndex, Distance len, T value, Comp comp) {
        /// 下沉操作
        /// 记录最开始的holeIndex，后续需要_push_heap
        Distance topIndex = holeIndex;
        /// 初始化secondChild为holeIndex，用于找到当前节点的右子节点
        Distance secondChild = holeIndex;

        /// 当当前节点有子节点时执行循环
        while (secondChild < (len - 1) / 2) {
            /// 计算当前节点的右子节点
            secondChild = 2 * (secondChild + 1);
            /// 如果左子节点更小则选择左子节点---需要检查越界访问
            if (secondChild < len && comp(*(first + secondChild), *(first + (secondChild - 1)))) {
                --secondChild;
            }
            *(first + holeIndex) = my::move(*(first + secondChild));
            holeIndex = secondChild;
        }

        /// 处理堆的长度为偶数且当前节点为最后一个有子节点的情况
        if ((len & 1) == 0 && secondChild == (len - 2) / 2) {
            secondChild = 2 * (secondChild + 1);        /// 右子节点
            *(first + holeIndex) = my::move(*(first + (secondChild - 1)));
            holeIndex = secondChild - 1;
        }

        /// 将新的值插入到调整后的堆中
        my::_push_heap(first, holeIndex, topIndex, my::move(value), comp);
    }

    /**
     * _pop_heap 用于从堆顶移除元素并保持堆的性质， 允许自定义比较函数来定义堆的性质
     *
     * 参数：
     *      first - 指向堆顶元素的迭代器
     *      last- 指向堆序列结束的迭代器
     *      result- 指向要存放被移除的堆顶元素的位置的迭代器
     *      comp-比较函数
     * */
    template<typename RandomIt, typename Comp>
    void _pop_heap(RandomIt first, RandomIt last, RandomIt result, Comp comp) {
        using T = typename std::iterator_traits<RandomIt>::value_type;
        using Distance = typename std::iterator_traits<RandomIt>::difference_type;

        /// 将原来堆尾的元素移动到堆顶--->将堆顶的元素和末尾元素交换位置
        T value = std::move(*result);
        *result = std::move(*first);

        /// 传入_adjust_heap重新调整，分别传入 first作为起始迭代器，0作为需要调整的节点索引
        /// last - first作为堆中的元素总和，以及移动后的value作为被调整节点的新的值
        my::_adjust_heap(first, Distance(0), Distance(last - first), my::move(value), comp);
    }

    /// 对pop_back做了进一步封装，默认比较器版本
    template<typename RandomIt>
    void pop_heap(RandomIt first, RandomIt last) {
        if (last - first > 1) {
            /// 需要调整
            --last;
            /// 这里在交换之后需要调整的元素就是最后一个节点即last
            my::_pop_heap(first, last, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
        }
    }

    /// pop_heap的封装，自定义比较器版本呢
    template<typename RandomIt, typename Comp>
    void pop_heap(RandomIt first, RandomIt last, Comp comp) {
        if (last - first > 1) {
            /// 需要调整
            --last;
            /// 这里在交换之后需要调整的元素就是最后一个节点即last
            my::_pop_heap(first, last, last, comp);
        }
    }

    /**
     * _make_heap 是一个模板函数，用于将给定的范围的元素重新组织成一个堆
     * 参数：
     *      first-指向序列开始的迭代器
     *      last-指向序列结束的迭代器
     *      comp-比较函数对象
     *
     * */
    template<typename RandomIt, typename Comp>
    void _make_heap(RandomIt first, RandomIt last, Comp comp) {
        /// 如果序列长度小于2,不需要处理
        if (last - first < 2) {
            return;
        }

        using T = typename std::iterator_traits<RandomIt>::value_type;
        using Distance = typename std::iterator_traits<RandomIt>::difference_type;

        /// 得到序列的最后一个非叶子节点的索引以及序列长度
        Distance len = last - first;
        Distance parent = (len - 2) / 2;

        /// 从最后一个节点开始往前对每个非叶子节点做下沉操作
        while (true) {
            /// 将当前节点做临时变量保存
            T value = std::move(*(first + parent));
            my::_adjust_heap(first, parent, len, std::move(value), comp);

            if (parent == 0) {
                /// 到根节点了
                return;
            }
            --parent;
        }
    }

    /// 进一步封装-->默认比较器版本
    template<typename RandomIt>
    void make_heap(RandomIt first, RandomIt last) {
        my::_make_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
    }

    /// 进一步封装-->自定义比较器版本
    template<typename RandomIt, typename Comp>
    void make_heap(RandomIt first, RandomIt last, Comp comp) {
        my::_make_heap(first, last, comp);
    }


    /// 堆排序的模板---排序操作会会破坏原始堆结构
    template<typename RandomIt, typename Comp>
    void _heap_sort(RandomIt first, RandomIt last, Comp comp) {
        while (last - first > 1) {
            --last;
            my::_pop_heap(first, last, last, comp);
        }
    }

    /// 封装排序堆-->默认比较器
    template<typename RandomIt>
    void sort_heap(RandomIt first, RandomIt last) {
        my::_heap_sort(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
    }

    /// 封装排序堆-->自定义比较器
    template<typename RandomIt, typename Comp>
    void sort_heap(RandomIt first, RandomIt last, Comp comp) {
        my::_heap_sort(first, last, comp);
    }

}

