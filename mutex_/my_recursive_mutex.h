/**
 * @file      my_recursive_mutex.h
 * @brief     [实现了跨平台的非递归互斥锁，提供了基础的互斥同步功能]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/**
 * 使用条件编译，根据不同的底层类型，实现对应的非递归互斥锁
 */
#ifdef _WIN32

#include <windows.h>
#include <process.h>

#else
#include <pthread.h>
#endif

#include <stdexcept>

namespace my {

#ifdef _WIN32
    using my_mutex_t = CRITICAL_SECTION;                /// windows CRITICAL_SECTION 互斥锁基本类型
    using my_recursive_mutex_t = CRITICAL_SECTION;
    using my_cond_t = CONDITION_VARIABLE;
    using my_thread_t = HANDLE;
    using my_pthread_once_t = INIT_ONCE;
    using my_pthread_key_t = DWORD;
#else
    using my_mutex_t = pthread_mutex_t;
    using my_recursive_mutex_t = pthread_mutex_t;
    using my_cond_t = pthread_cond_t;
    using my_thread_t = pthread_t;
    using my_pthread_once_t = pthread_once_t;
    using my_pthread_key_t = pthread_key_t;
#endif

    /**
     * 使用条件编译实现不同平台下的mutex普通锁实现
     */
    class mutex {
    public:
        /// 构造函数，初始化临界区的资源
        mutex() {
#ifdef _WIN32
            /// Windows 使用InitializeCriticalSection创建并初始化临界区，这是windows特有的用户态同步机制
            InitializeCriticalSection(&mutex_);
#else
            /// 初始化非递归互斥锁
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);     /// 指定PTHREAD_MUTEX_NORMAL类型（非递归，同一线程重复获取会导致死锁）
            if (pthread_mutex_init(&mutex_, &attr) != 0) {
                throw std::runtime_error("Failed to initialize mutex");
            }
            pthread_mutexattr_destroy(&attr);       /// 完成初始化后销毁属性对象释放资源
#endif
        }

        /// 析构函数，负责释放临界区的资源
        ~mutex() {
#ifdef _WIN32
            /// 释放临界区资源
            DeleteCriticalSection(&mutex_);
#else
            /// 销毁互斥锁
            pthread_mutex_destroy(&mutex_);
#endif
        }

        /// 非递归互斥锁的拷贝复制和移动拷贝移动赋值都被删除，避免锁的拷贝
        mutex(const mutex &) = delete;

        mutex &operator=(const mutex &) = delete;

        /// 移动构造和移动赋值
        mutex(mutex &&) = delete;

        mutex &operator=(mutex &&) = delete;


        /// 提供接口方法 lock() --> 进入临界区加锁，阻塞等待直到获取锁
        void lock() {
#ifdef _WIN32
            EnterCriticalSection(&mutex_);
#else
            pthread_mutex_lock(&mutex_);
#endif
        }

        /// unlock() --> 离开临界区，释放锁，唤醒等待线程
        void unlock() {
#ifdef _WIN32
            LeaveCriticalSection(&mutex_);
#else
            pthread_mutex_unlock(&mutex_);         /// 调用 posix的解锁函数
#endif
        }

        /// try_lock() --> 非阻塞尝试获取锁，成功返回true，失败立即返回false
        bool try_lock() {
#ifdef _WIN32
            return TryEnterCriticalSection(&mutex_) != 0;
#else
            return pthread_mutex_trylock(&mutex_) == 0;
#endif
        }

        /// 提供访问底层原生对象的接口， 在需要平台特定高级操作时（如与其他同步机制结合），可以直接操作底层对象
        using native_handle_type = my_mutex_t *;

        native_handle_type native_handle() {
            return &mutex_;
        }

    private:
        my_mutex_t mutex_;
    };

    /**
     * 递归互斥锁允许同一线程多次获取锁（需对应次数的释放），适用于递归函数等场景。
     */
    class recursive_mutex {
    public:
        /// 构造函数
        recursive_mutex() {
#ifdef _WIN32
            /// Windows的 CRITICAL_SECTION 默认就是递归的
            InitializeCriticalSection(&mutex_);
#else
            /// unix系统下初始化递归互斥锁
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);     /// 指定PTHREAD_MUTEX_RECURSIVE类型（递归，同一线程重复获取不会导致死锁）
            if (pthread_mutex_init(&mutex_, &attr) != 0) {
                throw std::runtime_error("Failed to initialize recursive mutex");
            }
            pthread_mutexattr_destroy(&attr);       /// 完成初始化后销毁属性对象释放资源
#endif
        }

        /// 析构函数，负责释放临界区的资源
        ~recursive_mutex() {
#ifdef _WIN32
            /// 释放临界区资源
            DeleteCriticalSection(&mutex_);
#else
            /// 销毁互斥锁
            pthread_mutex_destroy(&mutex_);
#endif
        }

        /**
         * 互斥锁是不可复制、不可移动的资源
         *      原因：
         *      复制锁会导致同步逻辑混乱
         *      移动锁可能导致原锁对象处于无效状态
         */

        recursive_mutex(const recursive_mutex &) = delete;

        recursive_mutex &operator=(const recursive_mutex &) = delete;

        /// 移动构造和移动赋值
        recursive_mutex(recursive_mutex &&) = delete;

        recursive_mutex &operator=(recursive_mutex &&) = delete;

        /**
         * 递归锁的lock()/unlock()操作与普通锁接口相同，但内部行为不同
         *      内部维护 "获取计数"，每次lock()计数 + 1，unlock()计数 - 1
         *      只有当计数减到 0 时，才真正释放锁，允许其他线程获取
         */
        void lock() {
#ifdef _WIN32
            EnterCriticalSection(&mutex_);
#else
            ///
            if (pthread_mutex_lock() != 0) {
                throw std::runtime_error("recursive mutex lock failed");
            }
#endif
        }

        void unlock() {
#ifdef _WIN32
            /// win平台下与Mutex一致
            LeaveCriticalSection(&mutex_);
#else
            if (pthread_mutex_unlock(&mutex_) != 0) {
            throw std::runtime_error("recursive mutex unlock failed");
        }
#endif
        }

        bool try_lock() {
#ifdef _WIN32
            return TryEnterCriticalSection(&mutex_) != 0;
#else
            return pthread_mutex_trylock(&mutex_) == 0;
#endif
        }

        /// 提供访问底层原生对象的接口， 在需要平台特定高级操作时（如与其他同步机制结合），可以直接操作底层对象
        using native_handle_type = my_recursive_mutex_t *;

        native_handle_type native_handle() {
            return &mutex_;
        }


    private:
        my_recursive_mutex_t mutex_;        /// 递归互斥锁变量
    };


}


