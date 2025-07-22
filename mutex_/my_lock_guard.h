/**
 * @file      my_lock_guard.h
 * @brief     [lock_guard实现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mutex_/my_recursive_mutex.h"

namespace my {

    template<typename Mutex_>
    class lock_guard {
    public:
        /// 定义别名
        using mutex_type = Mutex_;

        explicit lock_guard(mutex_type &mutex) : mutex_(mutex) {
            /// 构造时加锁
            mutex_.lock();
        }

        ~lock_guard() {
            /// 析构时解锁，遵循RAII思想
            mutex_.unlock();
        }

        /// 禁止锁的拷贝和复制
        lock_guard(const lock_guard &) = delete;

        lock_guard &operator=(const lock_guard &) = delete;

    private:
        Mutex_& mutex_;
    };
}