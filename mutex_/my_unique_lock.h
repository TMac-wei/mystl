/**
 * @file      my_unique_lock.h
 * @brief     [unique_lock]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "my_recursive_mutex.h"

namespace my {

    /// defer_lock_t，后面再加锁
    /// try_to_lock_t，尝试加锁
    /// adopt_lock_t，假设已经加锁

    enum class adopt_lock_t {
        adopt_lock
    };

    enum class defer_lock_t {
        defer_lock
    };

    enum class try_to_lock_t {
        try_to_lock
    };


    template<typename Mutex_>
    class unique_lock {
    public:
        using mutex_type = Mutex_;

        /// 默认构造函数, 不关联任何互斥锁的unique_lock对象
        unique_lock() noexcept: mutex_(nullptr), owns_(false) {}

        /// 关联到一个互斥锁并且立即获取它（阻塞等待直到获取成功）
        /// 使用std::addressof()安全的获取到对象的原始内存地址，不受重载&的影响
        explicit unique_lock(mutex_type &mutex) : mutex_(std::addressof(mutex)), owns_(true) {
            mutex_->lock();
        }

        /// adopt_lock策略构造函数，假设调用者已经获取到锁，只是转移锁的所有权给unique_lock
        unique_lock(mutex_type &mutex, adopt_lock_t) noexcept
                : mutex_(std::addressof(mutex)), owns_(true) {}

        /// 关联到互斥锁但不立即获取，需要后续手动调用lock()或try_lock()
        unique_lock(mutex_type &mutex, defer_lock_t) noexcept
                : mutex_(std::addressof(mutex)), owns_(false) {}

        /// 尝试获取锁，但不阻塞，如果成功获取则owns_为true，否则为false。
        unique_lock(mutex_type &mutex, try_to_lock_t)
                : mutex_(std::addressof(mutex)), owns_(mutex_->try_lock()) {}

        /// 析构  RAII
        ~unique_lock() {
            if (owns_) {    /// 如果持有锁，就释放
                mutex_->unlock();
            }
        }

        /// unique_lock是独占所有权的，不能被拷贝
        unique_lock(const unique_lock &) = delete;

        unique_lock &operator=(const unique_lock &) = delete;

        /// 移动构造和移动赋值运算
        unique_lock(unique_lock &&other) noexcept
                : mutex_(other.mutex_), owns_(other.owns_) {
            /// 清理 other 的所资源
            other.mutex_ = nullptr;
            other.owns_ = false;
        }

        unique_lock &operator=(unique_lock &&other) noexcept {
            if (owns_) {
                mutex_->unlock();
            }

            mutex_ = other.mutex_;
            owns_ = other.owns_;

            other.mutex_ = nullptr;
            other.owns_ = false;
            return *this;
        }

        /// 获取或操作
        void lock() {
            if (owns_) {
                throw std::runtime_error("unique_lock already owns a mutex");
            }
            if (!mutex_) {
                throw std::runtime_error("unique_lock has no associated mutex");
            }
            mutex_->lock();
            owns_ = true;
        }

        void unlock() {
            if (!owns_) {
                throw std::runtime_error("unique_lock does not own a mutex");
            }
            if (!mutex_) {
                throw std::runtime_error("unique_lock has no associated mutex");
            }
            mutex_->unlock();
            owns_ = false;
        }

        /// 尝试获取锁（非阻塞）
        bool try_lock() {
            if (owns_) {
                throw std::runtime_error("unique_lock already owns a mutex");
            }
            if (!mutex_) {
                throw std::runtime_error("unique_lock has no associated mutex");
            }
            /// 产生获取锁
            owns_ = mutex_->try_lock();
            return owns_;
        }

        /// 是否拥有锁
        bool owns_lock() const noexcept {
            return owns_;
        }

        /// 获取关联的互斥锁指针
        mutex_type *mutex() const noexcept {
            return mutex_;
        }

        /// 释放对互斥锁的关联
        mutex_type *release() noexcept {
            mutex_type *result = mutex_;
            mutex_ = nullptr;
            owns_ = false;
            return result;
        }


    private:
        Mutex_ *mutex_;         /// 指向关联的互斥锁
        bool owns_;             /// 是否拥有锁的所有权

    };


}
