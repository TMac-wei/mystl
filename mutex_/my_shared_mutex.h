/**
 * @file      my_shared_mutex.h
 * @brief     [共享互斥锁（shared_mutex）]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mutex_/my_recursive_mutex.h"
#include "my_condition_variable.h"

namespace my {

    /**
     * 共享互斥锁，允许两种类型的访问
     *  1、独占访问（写操作）-一次只能有一个线程进行
     *  2、共享访问（读操作）-可以有多个线程同时进行
     */
    struct shared_mutex_base {
        mutex mutex_;
        condition_variable gate1_;     /// 写条件变量
        condition_variable gate2_;     /// 读条件变量
        unsigned state_;       /// 标识当前锁的状态

        using native_handle_type = mutex::native_handle_type;
        /// 最高位标识有写操作正在进行或者等待
        static const unsigned kWriteEntered_ = 1u << (sizeof(unsigned) * 8 - 1);
        static const unsigned knReaders_ = ~kWriteEntered_;

        /// 构造函数
        shared_mutex_base() : state_(0) {}

        ~shared_mutex_base() = default;

        /// 写操作相关方法
        void lock() {
            /// 利用unique_lock加锁
            unique_lock<mutex> lock(mutex_);
            /// 等待直到没有写操作进行-->标记写操作进入 -> 等待所有读操作完成
            while (state_ & kWriteEntered_) {
                gate1_.wait(lock);
            }

            /// 标记写操作进入
            state_ |= kWriteEntered_;
            /// 等待所有读操作完成
            while (state_ & knReaders_) {
                gate2_.wait(lock);
            }
        }

        ///  尝试获取独占锁, 只有当没有任何读或写操作时，才获取锁, 非阻塞操作，立即返回结果
        bool try_lock() {
            unique_lock<mutex> lock(mutex_);
            if (state_ == 0) {
                state_ = kWriteEntered_;
                return true;
            }
            return false;
        }

        /// 释放独占锁
        void unlock() {
            unique_lock<mutex> lock(mutex_);
            state_ = 0;
            gate1_.notify_all();
        }

        /// 以下是关于读共享锁的获取, 许多个线程同时获取该锁以进行读操作
        void lock_shared() {
            unique_lock<mutex> lock(mutex_);
            /// 检查是否有写操作正在进行或者等待（写操作的优先级高于读操作）； 检查当前读操作数量是否达到最大
            while ((state_ & kWriteEntered_) || (state_ & knReaders_) == knReaders_) {
                gate1_.wait(lock);
            }

            /// 更新读者计数
            unsigned num_readers = (state_ & knReaders_) + 1;
            /// 清除当前的读者计数部分，然后将新的读者计数写入状态变量,实现读者数量的原子递增
            state_ &= ~knReaders_;
            state_ |= num_readers;
        }

        /// 尝试获取共享锁
        bool try_lock_shared() {
            unique_lock<mutex> lock(mutex_);
            unsigned num_readers = state_ & knReaders_;        /// 获取读者数量
            /// 当没有写操作，并且读计数没有达到最大值时，添加读者计数，并且更新读者计数
            if (!(state_ & kWriteEntered_) && num_readers != knReaders_) {
                ++num_readers;
                state_ &= ~knReaders_;
                state_ |= num_readers;
                return true;
            }
            return false;
        }

        /// 释放共享锁
        void unlock_shared() {
            /// 更新读者计数并根据当前状态决定是否通知等待的线程。
            unique_lock<mutex> lock(mutex_);
            /// 更新读者计数
            unsigned num_reader = (state_ & knReaders_) - 1;
            state_ &= ~knReaders_;
            state_ |= num_reader;
            /// 以上三行实现读者数量的原子递减

            /// 判断是否需要通知等待线程
            if (state_ & kWriteEntered_) {  /// 有写操作在等待
                if (num_reader == 0) {
                    gate2_.notify_one();  /// 当读者数量减到 0 时，通知等待的写线程可以继续了
                }
            } else {
                /// 没有写 操作等待
                if (num_reader == knReaders_ - 1) {
                    gate1_.notify_one();
                }
            }
        }
    };


    /// 共享锁的封装
    class shared_mutex {
    public:
        shared_mutex() = default;

        ~shared_mutex() = default;

        shared_mutex(const shared_mutex &) = delete;

        shared_mutex &operator=(const shared_mutex &) = delete;

        /// 对shared_mutex_base中的操作进行进一步封装
        void lock() {
            base_mutex_.lock();
        }

        void unlock() {
            base_mutex_.unlock();
        }

        bool try_lock() {
            return base_mutex_.try_lock();
        }

        void lock_shared() {
            base_mutex_.lock_shared();
        }

        void unlock_shared() {
            base_mutex_.unlock_shared();
        }

        bool try_lock_shared() {
            return base_mutex_.try_lock_shared();
        }

        /// 原生句柄
        using native_handle_type = shared_mutex_base::native_handle_type;

        native_handle_type native_handle() {
            /// 实际是返回最底层的互斥锁
            return base_mutex_.mutex_.native_handle();
        }

    private:
        shared_mutex_base base_mutex_;
    };
}