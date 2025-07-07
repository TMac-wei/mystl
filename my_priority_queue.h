/**
 * @file      my_priority_queue.h
 * @brief     [my_priority_queue]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include "my_heap_sort.h"
#include "my_vector.h"
#include "my_utility.h"

namespace my {
    template<typename T, typename Container = my::vector<T>,
            typename Compare = std::less<typename Container::value_type>>
    class priority_queue {
    public:
        using value_type = T;
        using size_type = typename Container::size_type;

    public:
        /// 构造函数
        explicit priority_queue(const Compare &comp = Compare(),
                                  const Container &container = Container())
                : comp_(comp), container_(container) {
            my::make_heap(container_.begin(), container_.end(), comp_);
        }

/// rules of five
        priority_queue(priority_queue &&) = default;

        priority_queue &operator=(priority_queue &&) = default;

        priority_queue(const priority_queue &) = default;

        priority_queue &operator=(const priority_queue &) = default;


        /// 输入迭代器版本的构造函数
        template<typename InputIterator>
        priority_queue(InputIterator first,
                         InputIterator last,
                         const Compare &comp = Compare(),
                         const Container &container = Container())
                : comp_(comp), container_(first, last) {
            my::make_heap(container_.begin(), container_.end(), comp_);
        }

        bool empty() const {
            return container_.empty();
        }

        size_type size() const {
            return container_.size();
        }

        const value_type &top() const {
            return container_.front();
        }

        value_type &top() {
            return container_.front();
        }

        void push(const value_type &value) {
            /// 先把元素压入容器，然后用容器压入堆中
            container_.push_back(value);
            my::push_heap(container_.begin(), container_.end(), comp_);
        }

        void pop() {
            /// 调用底层的堆
            my::pop_heap(container_.begin(), container_.end(), comp_);
            /// 弹出尾部元素
            container_.pop_back();
        }

        void push(value_type &&value) {
            /// 压入右值也是一样，先将右值押入到容器，然后通过容器进行入堆操作
            container_.push_back(std::move(value));
            my::push_heap(container_.begin(), container_.end(), comp_);
        }

        void swap(priority_queue &other) {
            using my::swap;
            swap(comp_, other.comp_);
            swap(container_, other.container_);
        }

        /// emplace这里要使用万能引用+可变长参数和完美转发
        template<typename...Args>
        void emplace(Args &&...args) {
            container_.emplace_back(std::forward<Args>(args)...);
            /// 调用底层的入栈进行压入
            my::push_heap(container_.begin(), container_.end(), comp_);
        }

    private:
        Compare comp_;
        Container container_;
    };

}