/**
 * @file      my_condition_variable.h
 * @brief     [条件变量类的简单实现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <chrono>
#include <stdexcept>
#include <windows.h>
#include <iostream>

#include "my_mutex.h"

namespace my {

    /// 等待状态枚举，与标准库保持一致
    enum class cv_status {
        no_timeout,     /// 未超时
        timeout         /// 超时唤醒
    };

    /**
     * condition_variable 类，用于多线程同步，条件变量允许一个线程等待另一个线程满足特定条件后再继续执行
     */
    class condition_variable {
    public:
        /// 构造函数，初始化windows条件变量
        condition_variable() {
            InitializeConditionVariable(&cond_);
        }

        /// 析构函数
        ~condition_variable() = default;

        /// 禁用拷贝构造和赋值(条件变量不可拷贝）
        condition_variable(const condition_variable &) = delete;

        condition_variable &operator=(const condition_variable &) = delete;

        /// 唤醒等待线程
        void notify_one() noexcept {
            /// 唤醒其中一个等待线程
            WakeConditionVariable(&cond_);
        }

        void notify_all() noexcept {
            /// 唤醒所有等待的线程
            WakeAllConditionVariable(&cond_);
        }

        /// wait，无限期等待，必须与unique_lock配合使用
        void wait(my::unique_lock<my::mutex> &lock) {
            /// 首先检查锁的状态，只有持有锁才能等待
            if (!lock.owns_lock()) {
                throw std::runtime_error("wait requires locked mutex");
            }

            /// 调用底层的 SleepConditionVariableCS 自动释放锁，并且在唤醒时重新获取
            if (!SleepConditionVariableCS(&cond_, lock.mutex()->native_handle(), INFINITE)) {
                throw std::runtime_error("condition variable wait failed");
            }
        }

        /// 带谓词的无限期等待：处理虚假唤醒
        template<class Predicate_>
        void wait(my::unique_lock<my::mutex> &lock, Predicate_ pred_) {
            /// 循环检查谓词，知道满足条件
            while (!pred_()) {
                /// 调用wait普通函数，只是多了循环检查谓词的过程
                wait(lock);
            }
        }

        /// 带绝对时间的等待
        template<class Clock_, class Duration_>
        cv_status wait_until (my::unique_lock<my::mutex> &lock,
                            const std::chrono::time_point<Clock_, Duration_> &abs_time) {
            /// 必须是持有锁才可以
            if (!lock.owns_lock()) {
                throw std::runtime_error("wait requires locked mutex");
            }

            /// 计算当前时间到目标时间的毫秒数，如果已经小于当前时间说明已经超时了，否则没有
            auto now = Clock_::now();
            if (abs_time <= now) {
                /// 已经超时了
                return cv_status::timeout;
            }

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(abs_time - now);
            if (duration > std::chrono::milliseconds(INT_MAX)) {
                duration = std::chrono::milliseconds(INT_MAX);
            }
            DWORD time_ms = static_cast<DWORD>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

            /// 等待指定时间
            BOOL result = SleepConditionVariableCS(&cond_, lock.mutex()->native_handle(), time_ms);
            if (!result) {
                DWORD error = GetLastError();
                if (error == WAIT_TIMEOUT || error == ERROR_TIMEOUT) {
                    return cv_status::timeout;
                } else {
                    /// 详细打印错误码，辅助调试
                    std::cerr << "SleepConditionVariableCS failed: " << error << std::endl;
                    throw std::runtime_error("condition variable wait_until failed");
                }
            }

            return cv_status::no_timeout;
        }

        /// 带谓词和绝对时间的等待
        template <typename Clock_, typename Duration_, typename Predicate_>
        bool wait_until (my::unique_lock<my::mutex> &lock,
                       const std::chrono::time_point<Clock_, Duration_> &abs_time,
                       Predicate_ pred_) {
            /// 循环谓词检测，处理虚假唤醒
            while (!pred_()) {
                if (wait_until (lock, abs_time) == cv_status::timeout) {
                    /// 超时了，但会当前谓词状态
                    return pred_();
                }
            }
            return true;
        }

        /// 带谓词和绝对时间的等待
        template<class Rep_, class Period_>
        cv_status wait_for(my::unique_lock<my::mutex> &lock,
                           const std::chrono::duration<Rep_, Period_> &rel_time) {
            /// 转换为绝对时间（当前时间+等待时间），复用 wait_until
            return wait_until (lock, std::chrono::steady_clock::now() + rel_time);
        }

        /// 原生句柄类型定义
        using native_handle_type = PCONDITION_VARIABLE;

        /// 获取原生句柄
        native_handle_type native_handle() noexcept {
            return &cond_;
        }


    private:
        CONDITION_VARIABLE  cond_;
    };
}
