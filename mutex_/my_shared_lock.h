/**
 * @file      my_shared_lock.h
 * @brief     [共享锁包装类 shared_lock]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mutex_/my_shared_mutex.h"

namespace my {
    /**
     * shared_lock 用于管理共享互斥锁（shared_mutex）的共享所有权，通常与读操作关联。
     */
    template<typename Mutex_>
    class shared_lock {
    public:
        using mutex_type = Mutex_;

        /// 默认构造函数
        shared_lock() : mtx_(nullptr), owns_(false) {}

        /// 关联互斥锁并获得共享锁
        explicit shared_lock(mutex_type &mutex) : mtx_(std::addressof(mutex)) {
            mtx_->lock_shared();        /// 获取共享锁
            owns_ = true;
        }

        /// 关联已经持有的互斥锁（adopt_lock_t)
        /// 仅关联互斥锁，不主动调用 lock_shared()，直接将 owns_ 设为 true
        shared_lock(mutex_type &mutex, adopt_lock_t) noexcept
                : mtx_(std::addressof(mutex)), owns_(true) {}

        /// 关联互斥锁，但延迟枷锁
        shared_lock(mutex_type &mutex, defer_lock_t) noexcept
                : mtx_(std::addressof(mutex)), owns_(false) {}

        /// 产生获取共享锁
        shared_lock(mutex_type &mutex, try_to_lock_t) noexcept
                : mtx_(std::addressof(mutex)), owns_(mtx_->try_lock_shared()) {}

        /// 析构
        ~shared_lock() {
            /// 如果持有锁，就释放
            if (owns_) {
                mtx_->unlock_shared();
            }
        }

        /// shared_lock 是不可拷贝的，因为锁的所有权无法安全共享
        shared_lock(const shared_lock &) = delete;

        shared_lock &operator=(const shared_lock &) = delete;

        /// 移动构造和赋值
        shared_lock(shared_lock &&other) noexcept
                : mtx_(other.mtx_), owns_(other.owns_) {
            other.mtx_ = nullptr;
            other.owns_ = false;
        }

        shared_lock &operator=(shared_lock &&other) noexcept {
            /// 如果原本就持有锁，则先释放
            if (owns_) {
                mtx_->unlock_shared();
            }

            /// 资源转移
            mtx_ = other.mtx_;
            owns_ = other.owns_;

            other.mtx_ = nullptr;
            other.owns_ = false;
            return *this;
        }

        /// 锁状态的查询
        bool own_lock() const noexcept {
            return owns_;
        }

        /// 获取关联的互斥锁
        mutex_type *mutex() const noexcept {
            return mtx_;
        }

        /// 释放互斥锁关联，返回原互斥锁的指针，调用之后shared_mutex不在管理该锁的释放
        mutex_type *release() noexcept {
            mutex_type *result = mtx_;     /// 保存当前互斥锁指针
            mtx_ = nullptr;                 /// 解除关联
            owns_ = false;                   /// 不再持有锁
            return result;
        }

        /// 获取共享锁（阻塞）
        void lock() {
            if (owns_) {
                /// 已经持有锁了，抛出异常
                throw std::runtime_error("shared_lock already owns a mutex");
            }
            /// 未关联到互斥锁，也异常
            if (!mtx_) {
                throw std::runtime_error("shared_lock has no associated mutex");
            }
            mtx_->lock_shared(); /// 阻塞获取共享锁
            owns_ = true;
        }

        /// 尝试获取共享锁（非阻塞）
        bool try_lock() {
            if (owns_) {
                /// 已经持有锁了，抛出异常
                throw std::runtime_error("shared_lock already owns a mutex");
            }
            /// 未关联到互斥锁，也异常
            if (!mtx_) {
                throw std::runtime_error("shared_lock has no associated mutex");
            }
            mtx_->try_lock_shared(); /// 阻塞获取共享锁
            owns_ = true;
        }

        /// 释放共享锁
        void unlock() {
            /// 如果根本没有拥有锁则会异常
            if (!owns_) {
                throw std::runtime_error("shared_lock does not own a mutex");
            }
            if (!mtx_) {  /// 未关联互斥锁，抛异常
                throw std::runtime_error("shared_lock has no associated mutex");
            }
            mtx_->unlock_shared();
            owns_ = false;
        }

    private:
        mutex_type *mtx_;      /// 指向关联的共享互斥锁
        bool owns_;            /// 是否持有锁
    };
}
