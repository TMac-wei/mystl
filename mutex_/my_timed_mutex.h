/**
 * @file      my_timed_mutex.h
 * @brief     [超时功能的互斥锁 timed_mutex]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_recursive_mutex.h"
#include "my_condition_variable.h"

namespace my {

    /**
     * 与普通互斥锁相比，timed_mutex 支持在指定时间内尝试获取锁，避免线程无限期等待。
     */
    class timed_mutex {
    public:
        using mutex_t = my::mutex;                      /// 基础互斥锁类型
        using lock_guard_t = my::lock_guard<mutex_t>;   /// 锁守卫类型（RAII）
        using unique_lock_t = my::unique_lock<mutex_t>; /// 可移动的锁类型
        using try_to_lock_type = my::try_to_lock_t;     /// 尝试加锁标签类型
        using condition_variable_t = my::condition_variable; /// 条件变量类型

        static constexpr try_to_lock_type try_to_lock = try_to_lock_type(); /// 尝试加锁标签实例

        /// 构造函数与析构
        timed_mutex() : locked_(false) {}

        ~timed_mutex() {
            /// lock_guard_t 会在析构后自动释放 mtx_
            lock_guard_t _(mtx_);
        }

        /// 互斥锁是不可拷贝的，删除拷贝构造和赋值运算符
        timed_mutex(const timed_mutex &) = delete;

        timed_mutex &operator=(const timed_mutex &) = delete;

        /// 获取锁
        void lock() {
            unique_lock_t lock(mtx_);       /// 锁定内部互斥锁
            cv_.wait(lock, [this] {
                return !locked_;            /// 直到锁未被持有时获取
            });
            locked_ = true;
        }

        /// 非阻塞尝试获取锁
        bool try_lock() noexcept {
            unique_lock_t lock(mtx_, try_to_lock);      /// 非阻塞尝试锁定内部互斥锁
            if (lock.owns_lock() && !locked_) {             /// 若成功锁定内部锁且外部锁未被持有
                locked_ = true;
                return true;
            }
            return false;
        }

        /// 在指定时间段内尝试获取锁
        template<class Rep, class Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time) {
            return try_lock_until(std::chrono::steady_clock::now() + rel_time); /// 将相对时间转换为绝对时间进行处理
        }

        /// 在指定时间点前尝试获取锁
        template<class Clock, class Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time) {
            /// 初始化内部锁，确保线程安全
            unique_lock_t lk(mtx_);
            /// 初始化未超时标志，用steady_clock避免系统时间调整影响
            bool no_timeout = std::chrono::steady_clock::now() < abs_time;
            /// 只要锁被持有并且没有超时就等待
            while (locked_ && no_timeout) {
                no_timeout = cv_.wait_until(lk, abs_time, [this] {
                    return !locked_;
                });
            }

            /// 若锁可用，获取锁并返回成功
            if (!locked_) {
                locked_ = true;
                return true;
            }
            return false;       /// 超时或者锁不可用，返回失败
        }

        void unlock() {
            lock_guard_t _(mtx_);  /// 锁定内部互斥锁，确保线程安全
            locked_ = false;       /// 标记锁为未持有状态
            cv_.notify_one();      /// 通知一个等待的线程（唤醒尝试获取锁的线程）
        }

    private:
        mutex_t mtx_;       /// 内部互斥锁，保护状态变量
        condition_variable_t cv_;      /// 条件变量，用于线程等待/通知
        bool locked_;               /// 锁状态：true 表示已经锁定，false表示未锁定
    };
}
