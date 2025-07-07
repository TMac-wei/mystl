/**
 * @file      my_queue.h
 * @brief     [根据deque实现queue]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_deque.h"

/// queue依据底层的deque进行部分封装得到
namespace my {

    template<typename T, typename Sequence = my::deque<T>>
    class queue {
    public:
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;
        using container_type = Sequence;

        /// queue是去调用底层的deque实现
        bool empty() const {
            return container_.empty();
        }

        size_type size() const {
            return container_.size();
        }

        reference front() {
            return container_.front();
        }

        const_reference front() const {
            return container_.front();
        }

        reference back() {
            return container_.back();
        }

        const_reference back() const {
            return container_.back();
        }

        void push(const value_type &v) {
            container_.push_back(v);
        }

        void pop() {
            container_.pop_front();
        }

        /// 高端push
        template<typename ...Args>
        void emplace_back(Args &&... args) {
            container_.emplace_back(std::forward<Args>(args)...);
        }

        void swap(queue &other) {
            container_.swap(other.container_);
        }

    private:
        container_type container_;

    };
}