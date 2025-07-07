/**
 * @file      my_stack.h
 * @brief     [基于deque实现stack]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "my_deque.h"

namespace my {
    /// 基于deque的部分功能进行封装实现stack
    template<typename T, typename Sequence = my::deque<T>>
    class stack {
    public:
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;
        using container_type = Sequence;

        bool empty() const {
            return container_.empty();
        }

        size_type size() const {
            return container_.size();
        }

        reference top() {
            return container_.back();
        }

        const_reference top() const {
            return container_.back();
        }

        void push(const value_type &v) {
            container_.push_back(v);
        }

        void pop() {
            container_.pop_back();
        }

        template<typename... Args>
        void emplace(Args &&... args) {
            container_.emplace_back(std::forward<Args>(args)...);
        }

        void swap(stack &other) {
            container_.swap(other.container_);
        }


    private:
        container_type container_;
    };
}