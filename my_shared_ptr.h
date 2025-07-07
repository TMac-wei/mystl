/**
 * @file      my_shared_ptr.h
 * @brief     [my_shared_ptr]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <atomic>
#include <memory>

namespace my {
    /// 引用控制块基类
    class sp_count_based {
    public:
        /// 默认构造函数，初始化引用计数，每当有一个shared_ptr时也必须要有一个weak_ptr的引用计数，防止其提前销毁
        sp_count_based() noexcept
                : use_count_(1), weak_count_(1) {}

        /// 作为基类，析构函数应该是虚函数
        virtual ~sp_count_based() noexcept {}

        /// dispose、destroy都是虚函数，由子类实现释放的逻辑
        /// 当use_count_ == 0时调用，释放被管理的对象
        virtual void dispose() noexcept = 0;

        /// 当weak_count_==0时调用，销毁整个控制块对象
        virtual void destroy() noexcept {
            delete this;
        }

        /// 获取被管理的原始指针，供外部 shared_ptr::get() 使用
        virtual void *ptr() = 0;

        /// 获取原始指针也需要const版本，在get()或者unique时需要const版本
        virtual const void *ptr() const = 0;

        /// 返回当前的引用计数
        int32_t use_count() const noexcept {
            return use_count_;
        }

        int32_t weak_count() const noexcept {
            return weak_count_;
        }

        //// 管理弱引用计数器
        void weak_add_ref() noexcept {
            /// 弱引用计数++
            ++weak_count_;
        }

        /// 若指针的释放
        void weak_release() noexcept {
            /// 这里释放的逻辑就是如果引用计数weak_count_释放到0了，说明应该释放掉了
            if (--weak_count_ == 0) {
                destroy();
            }
        }

        /// 管理use_count_引用计数，-1以及释放
        void add_ref_copy() noexcept {
            ++use_count_;
        }

        void release() noexcept {
            if (--use_count_ == 0) {
                dispose();      /// 这里销毁管理的资源
                weak_release(); /// 销毁控制块本身
            }
        }

        /// // 控制块基类中的 add_ref_lock 实现
        virtual bool add_ref_lock() {
            if (use_count_ == 0) {
                return false;
            }
            ++use_count_;
            return true;
        }

    private:
        /// 控制块是唯一对应一份资源的，禁止拷贝构造与赋值
        sp_count_based(const sp_count_based &) = delete;

        sp_count_based &operator=(const sp_count_based &) = delete;

        /// 引用控制块的成员变量主要是引用计数use_count与弱引用计数weak_count
        std::atomic<int32_t> use_count_;        /// 引用计数
        std::atomic<int32_t> weak_count_;        /// 弱引用计数

    };


    /// 接下来继承基类，自定义删除器
    template<typename Ptr, typename Deleter>
    class sp_count_deleter final : public sp_count_based {
    public:
        sp_count_deleter(Ptr ptr, Deleter deleter) noexcept
                : ptr_(ptr), deleter_(deleter) {}

        /// 同样删除器只允许一个对象持有，删除拷贝构造和赋值运算
        sp_count_deleter(const sp_count_deleter &) = delete;

        sp_count_deleter &operator=(const sp_count_deleter &) = delete;

        ~sp_count_deleter() noexcept {}

        /// 对dispose、destroy、ptr函数进行重写
        void *ptr() override {
            return ptr_;        /// 返回这个删除器管理的原始指针
        }

        /// const版本
        const void *ptr() const override {
            return ptr_;
        }

        /// dispose、destroy都可以继续被继承，所以写成虚函数
        virtual void dispose() noexcept override {
            if (ptr_) {
                deleter_(ptr_);     /// 调用删除器删除管理的对象
            }
        }

        /// 只有 sp_count_ptr_inplace是特例
        virtual void destroy() noexcept override {
            /// 销毁删除器对象
            delete this;
//            this->~sp_count_deleter();
        }

    private:
        Ptr ptr_;       /// 需要控制的指针，从外部指定模板参数
        Deleter deleter_;   /// 删除器
    };


    /// 实现 std::make_shared 时常用的内存缓冲区封装结构体，核心作用是：
    /// 提供对齐且足够大的未构造内存，用于构造对象但避免额外内存分配。
    /// 配合 make_shared 技术，它允许 shared_ptr 与实际对象共用一块内存，从而提升性能与缓存局部性
    struct sp_make_shared_tag {
    };

    /// 这里使用std::aligned_storage,确保分配的内存满足两个条件
    ///  1、可以容纳_Tp类型所需要的字节大小
    ///  2、满足_Tp类型的对齐要求，比如int对齐4字节，double对齐8字节等
    template<typename _Tp>
    struct __aligned_buffer {
        /// 定义一块大小与_Tp相同，对齐要求也满足_Tp的原始未经构造的内存区域
        typename std::aligned_storage<sizeof(_Tp), alignof(_Tp)>::type _M_storage;

        __aligned_buffer() = default;

        /// 提供一个特定标记构造方式（不做 value-initialization）,make_shared会使用这种方式去跳过一些构造函数
        __aligned_buffer(std::nullptr_t) {}

        /// 获取分配的原始未经构造的内存的地址
        void *_M_addr() noexcept {
            return static_cast<void *> (&_M_storage);
        }

        /// const版本
        const void *_M_addr() const noexcept {
            return static_cast<const void *> (&_M_storage);
        }

        /// 对_M_addr做进一步封装，直接返回需要的对象类型的指针
        _Tp *_M_ptr() noexcept {
            return static_cast<_Tp *>(_M_addr());
        }

        /// 为以上提供const版本
        const _Tp *_M_ptr() const noexcept {
            return static_cast<const _Tp *>(_M_addr());
        }
    };

    /// 接下来是std::make_shared实现机制的一部分，通过在已有的未经构造的原始内存上构造和管理控制块类
    /// sp_counted_ptr_inplace<Tp> 是专门用于 make_shared<T>(...) 的引用计数控制块，
    /// 它将 Tp 类型的对象直接构造在自身内置的内存区域中，避免了额外的堆分配（提升性能）。
    template<typename _Tp>
    class sp_count_ptr_inplace final : public sp_count_based {
    public:
        /// 成员变量包含上述__aligned_buffer分配的未经构造的内存空间
        __aligned_buffer<_Tp> storage_;

        /// placement new
        template<typename... Args>
        sp_count_ptr_inplace(Args... args) {
            std::allocator<_Tp> alloc;
            std::allocator_traits<std::allocator<_Tp>>::construct(
                    alloc,
                    static_cast<_Tp *>(ptr()),
                    std::forward<Args>(args)...
            );
            /// 以下的写法已经被C++17之后移除了
//            std::allocator<_Tp> alloc;
//            alloc.construct(static_cast<_Tp*>(ptr()), std::forward<Args>(args)...);
        }

        /// 禁止拷贝和赋值
        sp_count_ptr_inplace(sp_count_ptr_inplace const &) = delete;

        sp_count_ptr_inplace &operator=(sp_count_ptr_inplace const &) = delete;


        ~sp_count_ptr_inplace() noexcept {};

        /// 释放管理对象
        virtual void dispose() noexcept override {
            std::allocator<_Tp> alloc;
            std::allocator_traits<std::allocator<_Tp>>::destroy(
                    alloc,
                    static_cast<_Tp *>(ptr())
            );
            /// 调用allocator::destroy()手动析构_Tp对象
            /// 对应的是shared_ptr引用计数归零之后析构对象的动作
            /// 以下内容在c++17之后被移除
//            std::allocator<_Tp> alloc;
//            alloc.destroy(static_cast<_Tp*>(ptr()));
        }

        /// 释放控制块本身
        virtual void destroy() noexcept override {
            /// 当weak_ptr的引用计数归零之后，直接销毁控制块自己
            /// 为什么这里不是delete this？
            ///     因为sp_count_ptr_inplace是嵌入在make_shared里申请的大内存块中，
            ///     它与Tp对象共享同一块堆内存（sizeof(sp_counted_ptr_inplace<Tp>))
            ///     所以只是先销毁，后续由大块内存一起释放
            /// 释放控制块本身就直接调用析构函数
            this->~sp_count_ptr_inplace();
        }

        /// 返回被管理的对象
        void *ptr() noexcept override {
            return storage_._M_ptr();  // -> static_cast<Tp*>(storage_._M_addr())
        }

        const void *ptr() const noexcept override {
            return storage_._M_ptr();
        }
    };


    /// sp_count_ptr<Tp>控制块类，用于托管一个new创建的原始指针
    /**
     * shared_ptr<T>(new T) 构造；
     * new T 创建对象，原始指针传入 sp_count_ptr<T>;
     * shared_ptr 管理 sp_count_ptr<T>*，间接管理 T*;
     * 当所有 shared_ptr 销毁时，调用 dispose() 释放 T*;
     * 当所有 shared_ptr 和 weak_ptr 都销毁时，调用 destroy() 释放控制块本身
     * */
    template<typename Tp>
    class sp_count_ptr : public sp_count_based {
    public:
        /// 接受一个裸指针，存储到对应的私有成员ptr_上，管理这个裸指针的生命周期
        /// 不负责构造对象，只是接管已经现有的对象
        explicit sp_count_ptr(Tp *ptr) noexcept
                : ptr_(ptr) {}

        virtual ~sp_count_ptr() {}

        sp_count_ptr(const sp_count_ptr &) = delete;

        sp_count_ptr &operator=(const sp_count_ptr &) = delete;

        /// 获取管理的指针
        void *ptr() noexcept override {
            return ptr_;
        }

        const void *ptr() const noexcept override {
            return ptr_;
        }

        /// 这里也需要重写dispose和destroy函数
        virtual void dispose() noexcept override {
            /// 用于在最后一个shared引用释放时销毁对象
            /// 只释放托管对象，并不释放控制块本身
            if (ptr_) {
                delete ptr_;
            }
        }

        /// 用于释放控制块本身-->当 weak_count 降为 0 时触发
        ///  shared_ptr 和 weak_ptr 都消失后，销毁 sp_count_ptr 自身
        virtual void destroy() noexcept override {
            delete this;
        }

    private:
        Tp *ptr_;       /// 类型指针
    };

    /// 重载（） 自定义策略模式 + 仿函数对象性质来更好的完成删除器的工作
    struct __sp_array_delete {
        template<typename _Yp>
        void operator()(_Yp *__p) const {
            /// 删除数组对象
            delete[] __p;
        }
    };

    /// 引用计数管理类 shared_count
    /// 封装一个指向引用计数控制块(sp_count_base派生类）的裸指针，提供引用计数增加/减小、对象访问、赋值语义等功能
    /// shared_count是shared_ptr的隐藏实现，主要作用有如下几个：
    /// 1、持有对共享控制块(sp_count_base*)的所有权
    /// 2、管理shared引用计数、增加、减少、判断唯一性等
    /// 3、支持从make_sahred创建的inplace控制块
    /// 4、 可以从weak_ptr升级
    /// 5、 不直接持有对象指针，但是可以通过ptr()获得原始对象裸指针
    class weak_count;

    class shared_count {
    public:
        shared_count() noexcept
                : pi_(nullptr) {}

        /// 显式单参构造任意类型的计数管理类
        template<typename Tp>
        explicit shared_count(Tp *ptr)
                : pi_(new sp_count_ptr<Tp>(ptr))/// 这里pi是引用控制块的指针，所以需要将其构造成是一个sp_count_ptr
        {}

        /// 支持make_shared方式构造
        template<typename Tp, typename... Args>
        shared_count(sp_make_shared_tag, Tp *, Args &&... args)
                : pi_(nullptr) {
            /// 通过sp_count_ptr_inplace在已经存在的内存中构造对象
            using sp_cplace = sp_count_ptr_inplace<Tp>;
            pi_ = new sp_cplace(std::forward<Args>(args)...);
        }

        /// 析构对象
        ~shared_count() {
            if (pi_) {
                pi_->release();
            }
        }

        /// 这里需要在weak_count写完之后加上，这里直接访问的话会访问不完整的类型，
        /// 导致无法访问other.pi_
        shared_count(const weak_count &other) noexcept;

        shared_count(const shared_count &other) noexcept
                : pi_(other.pi_) {
            if (pi_) {
                pi_->add_ref_copy();    /// 增加引用计数
            }
        }

        shared_count &operator=(const shared_count &other) noexcept {
            if (pi_ != other.pi_) {
                /// 释放旧的管理的对象
                if (pi_) {
                    pi_->release();
                }
                pi_ = other.pi_;
                if (pi_) {
                    pi_->add_ref_copy();
                }
            }
            return *this;
        }

        void swap(shared_count &other) noexcept {
            std::swap(pi_, other.pi_);
        }

        int32_t use_count() const noexcept {
            /// 如果pi_管理的有对象，则调用use_count获取到当前的引用计数，否则为0
            return pi_ ? pi_->use_count() : 0;
        }

        bool unique() const noexcept {
            return use_count() == 1;
        }

        bool empty() const noexcept {
            return pi_ == nullptr;
        }

        /// 获取管理的ptr
        void *ptr() const noexcept {
            return pi_->ptr();
        }

    private:
        /// 可以从weak_ptr构造shared_ptr,所以weak_count是友元
        friend class weak_count;

        sp_count_based *pi_;    /// 控制块基类指针对象
    };


    /**
     * weak_count是一个用于实现std::weak_ptr的控制块句柄类，主要负责
     *  1、引用共享的控制块 sp_count_base* pi_
     *  2、增加和减少控制块中的weak_count
     *  3、支持从shared_count或其他weak_count拷贝或移动
     *  4、提供use_count() empty()等接口
     * */
    class weak_count {
    public:
        /// 默认构造
        weak_count() : pi_(nullptr) {}

        /// 显式从sahred_count构造对象，不支持隐式转换
        explicit weak_count(const shared_count &other) noexcept
                : pi_(other.pi_) {
            /// 如果pi_不为空，需要增加一次weak_count，即调用基类的weak_add_ref（）
            if (pi_) {
                pi_->weak_add_ref();
            }
        }

        /// 自身拷贝构造
        weak_count(const weak_count &other) noexcept
                : pi_(other.pi_) {
            if (pi_) {
                pi_->weak_add_ref();
            }
        }

        /// 赋值运算符 shared_count-->weak_count
        weak_count &operator=(const shared_count &other) noexcept {
            sp_count_based *temp = other.pi_;
            if (temp) {
                temp->weak_add_ref();///     新的引用计数+1
            }
            if (pi_) {
                pi_->weak_release();///     旧的引用计数-1
            }
            pi_ = temp;    /// 相当于是释放旧的，接收新的引用计数
            return *this;
        }

        /// 赋值运算符 weak_count-->weak_count
        weak_count &operator=(const weak_count &other) noexcept {
            if (pi_ != other.pi_) {
                if (pi_) {
                    pi_->weak_release();///     旧的引用计数-1
                }
                pi_ = other.pi_;   /// 接收新的引用控制块指针
                if (pi_) {
                    pi_->weak_add_ref();///     新的引用计数+1
                }
            }
            return *this;
        }

        /// 移动构造和移动赋值运算
        weak_count(weak_count &&other) noexcept
                : pi_(other.pi_) {
            other.pi_ = nullptr;
        }

        weak_count &operator=(weak_count &&other) noexcept {
            if (pi_ != other.pi_) {
                if (pi_) {
                    pi_->weak_release();
                }
                pi_ = other.pi_;
                other.pi_ = nullptr;
            }
            return *this;
        }

        ~weak_count() {
            /// 将引用计数减一，如果最后weak_count == 0 && shared_count == 0则释放掉控制块
            if (pi_) {
                pi_->weak_release();
            }
        }

        /// 两个控制块的内容交换
        void swap(weak_count &other) noexcept {
            std::swap(pi_, other.pi_);
        }

        /// 查看当前的引用计数
        int32_t use_count() const noexcept {
            return pi_ ? pi_->use_count() : 0;
        }

        bool empty() const noexcept {
            return pi_ == nullptr;
        }

    private:
        /// 允许从shared_count构造
        friend class shared_count;

        sp_count_based *pi_;       /// 基类指针，这个基类控制块具有引用计数以及析构释放对象和控制块的接口
    };

    /// 补齐shared_count中的由weak_count-->shared_count的函数
    shared_count::shared_count(const weak_count &other) noexcept
            : pi_(other.pi_) {
        /// 在从weak_ptr中提升为shared_ptr时，不能直接无条件的引用计数+1,需要检查资源是否还活着-->shared_count > 0
        /// 只有对象还活着时才允许引用计数+1
        if (pi_ && !pi_->add_ref_lock()) {
            /// 资源已经不存在了无法提升为shared_ptr
            pi_ = nullptr;
        }
    }

    /// weak_ptr的前置声明，访问shared_ptr得到从weak_ptr来的构造函数
    template<typename Tp>
    class weak_ptr;

    /**
     * shared-ptr主要管理两个核心内容
     * 1、 refcount_ ，这是一个shared_count对象，管理引用计数（底层为控制块 sp_count_base*)
     * 2、 ptr_,实际被管理的裸指针
     * */
    template<typename Tp>
    class shared_ptr {
    public:
        using element_type = Tp;

        /// 创建一个空的 shared_ptr，不会管理任何对象
        shared_ptr() noexcept: refcount_(), ptr_(nullptr) {}

        /// 从原始裸指针来构造sahred_ptr
        template<typename U>
        explicit shared_ptr(U *ptr)
                : refcount_(ptr), ptr_(ptr) {}

        /// make_shared方式构造
        /**
         * 使用make_shared方式构造，在一块内存中同时分配控制块和对象
         * sp_make_shared_tag表示标记类型，区分构造方式
         * 对象由recount_负责创建与销毁，ptr_是该对象的指针
         * */
        template<typename... Args>
        shared_ptr(sp_make_shared_tag tag, Args &&... args)
                : refcount_(tag, (element_type *) 0, std::forward<Args>(args)...), ptr_(nullptr) {
            ptr_ = static_cast<element_type *>(refcount_.ptr());
        }

        /// 从另一个shared_ptr构造
        /**
         * 这种构造方式允许多个shared_ptr贡献给i昂控制块，但ptr_指向不同的子对象
         * 常用于enable_sahred_from_this、结构体中的字段
         * 控制块相同，对象地址不同
         * */
        template<typename U>
        shared_ptr(const shared_ptr<U> &other, Tp *ptr) noexcept
                : refcount_(other.refcount_), ptr_(ptr) {}

        /// 自身的拷贝构造
        shared_ptr(const shared_ptr &other) noexcept = default;

        shared_ptr(shared_ptr &&other) noexcept
                : refcount_(std::move(other.refcount_))   /// 这里需要和other的进行交换，进行所有权的转移
                , ptr_(other.ptr_) {
            /// 如果这里采用这种方式的话，那么自身默认构造产生的控制块，在交换之后就不会被释放
//             refcount_.swap(other.refcount_);
///         shared_ptr(std::move(other)).swap(*this);
            other.ptr_ = nullptr;
        }

        /// 从weak_ptr转移
        template<typename U>
        shared_ptr(const weak_ptr<U> &other)
                : refcount_(other.refcount_), ptr_(other.ptr_) {}

        /// 允许从其他类型的shared_ptr构造
        template<typename U>
        shared_ptr(const shared_ptr<U> &other) noexcept
                : refcount_(other.refcount_), ptr_(other.ptr_) {
            static_assert(std::is_convertible<U *, Tp *>::value,
                          "shared_ptr<U> must be convertible to shared_ptr<Tp>");

        }

        /// 可接受Nullptr的赋值运算符
        shared_ptr &operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        /// 拷贝赋值-自身类型
        /// 指针和控制块都替换
        shared_ptr &operator=(const shared_ptr &other) noexcept {
            refcount_ = other.refcount_;
            ptr_ = other.ptr_;
            return *this;
        }

        /// 移动赋值
        shared_ptr &operator=(shared_ptr &&other) noexcept {
            /// 经典的复制交换实现,调用 swap 转移资源
            shared_ptr(std::move(other)).swap(*this);
            return *this;
        }

        /// 跨类型移动赋值
        template<typename U>
        shared_ptr &operator=(const shared_ptr<U> &other) noexcept {
            ptr_ = other.ptr_;
            refcount_ = other.refcount_;
            return *this;
        }

        template<typename U>
        shared_ptr &operator=(shared_ptr<U> &&other) noexcept {
            shared_ptr(std::move(other)).swap(*this);
            return *this;
        }

        /// 接口函数
        /// swap()交换两个shared_ptr的所有权
        void swap(shared_ptr &other) noexcept {
            std::swap(refcount_, other.refcount_);
            std::swap(ptr_, other.ptr_);
        }

        Tp *get() const noexcept {
            /// 返回当前控制的裸指针
            return ptr_;
        }

        void reset() noexcept {
            /// 构造一个空的 shared_ptr()，然后与当前对象交换资源
            /// 原对象的控制块引用计数减 1，必要时释放资源
            shared_ptr().swap(*this);
        }

        /// 如果传入的是一个其他类型的裸指针
        template<typename U>
        void reset(U *ptr) {
            /// 接管一个新的裸指针：创建一个新的 shared_ptr<U> 管理它
            /// 然后与当前对象交换资源，自动释放旧的资源、控制块
            shared_ptr<U>(ptr).swap(*this);
        }

        Tp &operator*() const noexcept {
            return *ptr_;
        }

        Tp *operator->() const noexcept {
            return ptr_;
        }

        int32_t use_count() const noexcept {
            return refcount_.use_count();
        }

        bool unique() const noexcept {
            /// 调用底层的shared_count unique方法
            return refcount_.unique();
        }


        explicit operator bool() const noexcept {
            /// 即使refcount_失效了也可能返回true，所以需要增加判断条件
            return ptr_ != nullptr;
        }

        /// 利用 shared_count 的析构函数自动处理引用计数递减、对象释放
        ~shared_ptr() = default;

    private:
        template<typename U>
        friend
        class weak_ptr;

        /// 需要在不同类型的shared_ptr之间访问refcount_，所以需要需要将shared_ptr<U>和shared_ptr<T>互相设计为友元
        template<typename U>
        friend
        class shared_ptr;

        shared_count refcount_;        ///引用计数管理器，控制对象生命周期 有多少个shared_count
        Tp *ptr_;      /// 实际管理的对象指针
    };

    //// make_shared
    template<typename Tp, typename...Args>
    shared_ptr<Tp> make_shared(Args &&...args) {
        return shared_ptr<Tp>(sp_make_shared_tag(),
                              std::forward<Args>(args)...);
    }


    /// 最后是weak_ptr  弱引用：不拥有对象，仅引用 shared_ptr 所管理的资源
    template<typename Tp>
    class weak_ptr {
    public:
        /// 默认构造函数
        weak_ptr() noexcept: refcount_(), ptr_(nullptr) {}

        /// 从shared_ptr构造 --> 建立对 shared_ptr 的弱引用:增加弱引用计数但不会增加强引用计数
        weak_ptr(const shared_ptr<Tp> &other) noexcept
                : refcount_(other.refcount_), ptr_(other.ptr_) {}

        /// 拷贝构造
        weak_ptr(weak_ptr const &other) = default;

        /// 移动构造
        weak_ptr(weak_ptr &&other) noexcept
                : refcount_(std::move(other.refcount_)), ptr_(other.ptr_) {
            /// 移动了另一个weak_ptr的对象之后要将对方的指针置为nullptr
            other.ptr_ = nullptr;
        }

        /// 模板拷贝构造，支持类型转换
        /// 支持 weak_ptr<Base> → weak_ptr<Derived>，只要类型可转
        template<typename U>
        weak_ptr(const weak_ptr<U> &other) noexcept
                : refcount_(other.refcount_), ptr_(other.ptr_) {}

        /// 右值模板移动构造
        template<typename U>
        weak_ptr(weak_ptr &&other) noexcept
                : refcount_(std::move(other.refcount_)), ptr_(other.ptr_) {
            other.ptr_ = nullptr;
        }

        /// 赋值运算符包括 拷贝赋值，移动赋值，模板赋值运算符
        weak_ptr &operator=(const weak_ptr &other) noexcept {
            ptr_ = other.ptr_;
            refcount_ = other.refcount_;
            return *this;
        }

        /// 移动赋值运算符
        weak_ptr &operator=(weak_ptr &&other) noexcept {
            /// 使用swap安全的交换资源
            weak_ptr(std::move(other)).swap(*this);
            return *this;
        }

        /// 模板赋值运算符,从其他类型的weak_ptr转换而来
        template<typename U>
        weak_ptr &operator=(const weak_ptr<U> &other) noexcept {
            ptr_ = other.ptr_;
            refcount_ = other.refcount_;
            return *this;
        }

        /// 移动赋值模板运算
        template<typename U>
        weak_ptr &operator=(weak_ptr &&other) noexcept {
            weak_ptr(std::move(other)).swap(*this);
            return *this;
        }


        /// 实用接口
        void swap(weak_ptr &other) {
            std::swap(ptr_, other.ptr_);
            std::swap(refcount_, other.refcount_);
        }

        /// 获取引用计数
        int32_t use_count() const noexcept {
            return refcount_.use_count();
        }

        /// 获取是否过期--->引用计数为减到0了
        bool expired() const noexcept {
            return refcount_.use_count() == 0;
        }

        /// 将weak_ptr升级为shared_ptr
        /// 若对象尚未被销毁，返回一个新的 shared_ptr<Tp>（共享控制器并延长生命周期）。
        /// 若对象已被销毁，返回一个空的 shared_ptr<Tp>
        shared_ptr<Tp> lock() const noexcept {
            if (refcount_.use_count() == 0) {
                return shared_ptr<Tp>();        /// 如果对象已经被销毁，返回空的shared_ptr
            }
            return shared_ptr<Tp>(*this);
        }


        /// 将当前 weak_ptr 重置为空（引用计数为 nullptr）
        void reset() noexcept {
            weak_ptr().swap(*this);
        }

        ///当对象未被销毁时返回 true，否则返回 false-->常用于条件判断
        explicit operator bool() const noexcept {
            return !expired();
        }

        ~weak_ptr() = default;

//     private:
//         void assign_(Tp* ptr, shared_count const& refcount)
//         {
//             if (use_count() == 0) {
//                 ptr_ = ptr;
//                 refcount_ = refcount;
//             }
//         }
    private:
        /// 成员变量
        weak_count refcount_;   /// 弱引用管理计数器
        Tp *ptr_;               /// 原始指针，指向实际资源，不参与析构

        template<typename> friend
        class shared_ptr;
    };


    //// 接下来是指针类型转换函数的详解

    /// static_pointer_cast->编译期类型安全转换（如 Base* → Derived*，需手动确认）
    /// 用于继承体系中向下转换
    template<typename Tp, typename Up>
    shared_ptr<Tp> static_pointer_cast(const shared_ptr<Up> &r) noexcept {
        return shared_ptr<Tp>(r, static_cast<Tp *>(r.get()));
    }

    /// dynamic_pointer_cast->运行时安全的类型转换，失败时会返回空 shared_ptr<Tp>
    /// 需要有虚函数表支持RTTI
    template<typename Tp, typename Up>
    shared_ptr<Tp> dynamic_pointer_cast(const shared_ptr<Up> &r) noexcept {
        return shared_ptr<Tp>(r, dynamic_cast<Tp *>(r.get()));
    }

    /// const_pointer_cast->去除 const 限定符，通常用于 const_cast 的替代
    template<typename Tp, typename Up>
    shared_ptr<Tp> const_pointer_cast(const shared_ptr<Up> &r) noexcept {
        return shared_ptr<Tp>(r, const_cast<Tp *>(r.get()));
    }
}

