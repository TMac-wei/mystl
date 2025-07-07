/**
 * @file      my_string.h
 * @brief     [std::string复现]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <cstring>
#include "memory_/my_allocator.h"
#include "utility_/my_move.h"
#include "my_algorithm.h"

namespace my {

    /// 短字符串优化
    template<typename charT, typename Allocator = MyAlloc<charT>>
    class basic_string {
    public:
        using self = basic_string;
        using value_type = charT;
        using allocator_type = Allocator;
        using size_type = typename allocator_type::size_type;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using iterator = pointer;
        using const_iterator = const_pointer;

        static constexpr size_type npos = -1;
    public:
        basic_string() {
            assert(sizeof(LongString) == sizeof(ShortString));
            set_short_size_(0);
        }

        basic_string(size_type size) {
            if (size > max_size()) {
                throw std::length_error("basic_string");
            }
            if (fits_in_sso_(size)) {  /// 短字符串
                set_short_size_(size);
            } else {  /// 长字符串
                auto capacity = recommend_(size);
                auto p = alloc_.allocate(capacity);
                set_long_size_(size);
                set_long_capacity_(capacity);
                set_long_pointer_(p);
            }
        }

        /// 拷贝构造
        basic_string(const basic_string &str) {
            init_(str.data(), str.size());
        }

        basic_string(const basic_string &_str, size_type _pos)
                : basic_string(_str, _pos, npos) {}

        /// 移动构造
        basic_string(basic_string &&str) noexcept {
            rep_ = str.rep_;
            str.rep_ = Rep();
            str.set_short_size_(0);
        }

        basic_string(basic_string &&str, size_type pos)
                : basic_string(my::move(str), pos, npos) {}

        /// 利用边长的value_type数组进行初始化
        basic_string(const value_type *s) {
            size_type size = strlen(s);
            init_(s, size);
        }

        basic_string(const value_type *s, size_type n) {
            init_(s, n);
        }

        basic_string(size_type n, value_type c) {
            init_(n, c);
        }

        basic_string(basic_string &&str, size_type pos, size_type n) {
            if (pos > str.size()) {
                throw std::out_of_range("basic_string");
            }
            auto len = str.size() - pos;
            if (n > len) {
                n = len;
            }
            init_(str.data() + pos, n);
        }

        ~basic_string() {
            if (is_long_()) {
                alloc_.deallocate(get_long_pointer_(), get_long_capacity_());
            }
        }

        /// 以下是一些常见的方法接口

        value_type *data() { return get_pointer_(); }

        const value_type *data() const { return get_pointer_(); }

        size_type size() const { return is_long_() ? get_long_size_() : get_short_size_(); }

        size_type length() const { return size(); }

        size_type capacity() const { return is_long_() ? get_long_capacity_() : kShortCap - 1; }

        bool empty() const { return size() == 0; }

        value_type *c_str() { return data(); }

        const value_type *c_str() const { return data(); }

        basic_string substr(size_type pos = 0, size_type n = npos) const { return basic_string(*this, pos, n); }

        reference operator[](size_type pos) { return data()[pos]; }

        const_reference operator[](size_type pos) const { return data()[pos]; }

        size_type max_size() const { return alloc_.max_size(); }

        /// 迭代器操作
        iterator begin() { return data(); }

        const_iterator cbegin() const { return data(); }

        iterator end() { return data() + size(); }

        const_iterator cend() const { return data() + size(); }

        iterator rbegin() { return data() + size() - 1; }

        const_iterator crbegin() const { return data() + size() - 1; }

        iterator rend() { return data() - 1; }

        const_iterator crend() const { return data() - 1; }

        /// 在容器中取元素
        reference front() {
            if (empty()) {
                throw std::out_of_range("basic_string");
            }
            return *get_pointer_();
        }

        const_reference front() const {
            if (empty()) {
                throw std::out_of_range("basic_string");
            }
            return *get_pointer_();
        }

        reference back() {
            if (empty()) {
                throw std::out_of_range("basic_string");
            }
            return *(get_pointer_() + size() - 1);
        }

        const_reference back() const {
            if (empty()) {
                throw std::out_of_range("basic_string");
            }
            return *(get_pointer_() + size() - 1);
        }

        reference at(size_type pos) {
            if (pos >= size()) {
                throw std::out_of_range("basic_string");
            }
            return data()[pos];
        }

        const_reference at(size_type pos) const {
            if (pos >= size()) {
                throw std::out_of_range("basic_string");
            }
            return data()[pos];
        }

        void clear() {
            data()[0] = value_type();
            set_size_(0);
        }

        /// 赋值操作
        basic_string& operator=(const basic_string& str) {
            if (this != &str) {
                if (is_long_()) {
                    alloc_.deallocate(get_long_pointer_(), get_long_capacity_());
                }
                init_(str.data(), str.size());
            }
            return *this;
        }

        basic_string& operator=(basic_string&& str) noexcept {
            if (this != &str) {
                if (is_long_()) {
                    alloc_.deallocate(get_long_pointer_(), get_long_capacity_());
                }
                rep_ = str.rep_;
                str.rep_ = Rep();
                str.set_short_size_(0);
            }
            return *this;
        }

        basic_string& operator=(const value_type* s) {
            size_type size = strlen(s);
            if (is_long_()) {
                alloc_.deallocate(get_long_pointer_(), get_long_capacity_());
            }
            init_(s, size);
            return *this;
        }

        basic_string& operator=(value_type c) {
            if (is_long_()) {
                alloc_.deallocate(get_long_pointer_(), get_long_capacity_());
            }
            init_(1, c);
            return *this;
        }

        /// 用各种方式将新值赋给字符串对象
        /// 将字符串内容设置为 count 个 _c 字符
        basic_string& assign(size_type count, value_type _c) {
            size_type cap = capacity();
            if (cap < count) {
                if (is_long_()) {
                    alloc_.deallocate(get_long_pointer_(), cap);
                }
                /// init_(count, _c) 是构造新数据
                init_(count, _c);
            } else {
                pointer p = get_pointer_();
                for (size_type i = 0; i < count; ++i) {
                    p[i] = _c;
                }
                p[count] = value_type();
                set_size_(count);
            }
            return *this;
        }

        /// 将字符串设置为 s[0] 到 s[n-1]
        /// 使用 init_(s, n) 构造新的字符串（会调用分配器）；
        /// 否则就直接写入当前 buffer；
        basic_string& assign(const value_type* s, size_type n) {
            size_type cap = capacity();
            if (cap < n) {
                if (is_long_()) {
                    alloc_.deallocate(get_long_pointer_(), cap);
                }
                init_(s, n);
            } else {
                pointer p = get_pointer_();
                memcpy(p, s, n);
                p[n] = value_type();
                set_size_(n);
            }
            return *this;
        }

        /// 拷贝赋值，等价于 operator=
        basic_string& assign(const basic_string& str) {
            *this = str;
            return *this;
        }

        /// 将一个右值字符串移动赋值给当前对象
        basic_string& assign(basic_string&& str) {
            *this = my::move(str);
            return *this;
        }

        ///  s[0..n-1] 追加到当前字符串末尾
        basic_string& append(const value_type* s, size_type n) {
            /// 获取当前的大小和总空间
            size_type cap = capacity();
            size_type size_ = size();
            if (cap - size_ - 1 >= n) {  /// 剩下的空间足够
                memcpy(data() + size_, s, n);
                set_size_(size_ + n);
                data()[size()] = value_type();  /// 这里必须写到新的size末尾，而不能是data()[size_] = value_type()，这样会导致截断
            } else {
                /// 剩下的空间不足够就会构建新的空间并且添加数据
                grow_by_and_add_(cap, size_ + n - cap, size_, n, s);
            }
            return *this;
        }

        /// 追加另一个 basic_string 的内容
        basic_string& append(const basic_string& str) {
            return append(str.data(), str.length());
        }

        /// 追加一个 C 风格字符串
        basic_string& append(const value_type* s) {
            return append(s, strlen(s));
        }

        /// 追加 count 个字符 c
        basic_string& append(size_type count, value_type c) {
            for (size_type i = 0; i < count; ++i) {
                push_back(c);
            }
            return *this;
        }

        /// 向字符串尾部添加一个字符
        void push_back(value_type c) {
            size_type cap = capacity();
            size_type size_ = size();
            if (size_ + 1 >= cap) {
                /// 只开辟新的空间，不进行赋值
                grow_by_(cap, 1, size_);
            }
            data()[size_] = c;
            set_size_(size_ + 1);
            data()[size()] = value_type();  /// 这里需要特别注意，写入末尾的元素时，必须要写到更新之后的size上
        }

        /// 重新设置大小
        void resize(size_type request_size) {
            size_type size_ = size();

            if (request_size < size_) {
            /// 如果需求大小 ＞ 当前的大小，则将对于的空间用默认的方式设置
                data()[request_size] = value_type();
                set_size_(request_size);
            } else if (request_size > size_) {
                /// 如果不够，需要分配新的空间
                size_type cap = capacity();
                if (cap < request_size) {
                    grow_by_(cap, request_size - size_, size_);
                }
                pointer p = get_pointer_();
                for (size_type i = size_; i <= request_size; ++i) {
                    p[i] = value_type();
                }
                set_size_(request_size);
            }
        }
    private:
        /// 实现SSO的basic_string
        struct LongString {
            pointer data;                                      /// 8
            size_type size;                                    /// 8
            size_type cap: sizeof(size_type) * CHAR_BIT - 1;  /// 64 - 1 = 63
            size_type is_long: 1;                             /// 1  标志位
        };

        static constexpr int32_t kShortCap = sizeof(LongString) / sizeof(value_type) - 1;

        struct ShortString {
            value_type data[kShortCap];
            unsigned char size: 7;
            unsigned char is_long: 1;
        };

        struct Rep {
            union {
                LongString lstr;
                ShortString sstr;
            };
        };

        Rep rep_;
        allocator_type alloc_;

    private:
        /// 以下是一些接口
        bool is_long_() const {
            return rep_.lstr.is_long == 1;
        }

        /// 是否采用了SSO
        bool fits_in_sso_(size_type n) {
            return n < kShortCap;
        }

        /// 设置字符参数
        void set_size_(size_type n) {
            if (is_long_()) {
                set_long_size_(n);
            } else {
                set_short_size_(n);
            }
        }

        /// 获取对应的地址，根据长短类型来判断返回哪种类型的地址
        pointer get_pointer_() {
            return is_long_() ? get_long_pointer_() : get_short_pointer_();
        }

        const_pointer get_pointer_() const {
            return is_long_() ? get_long_pointer_() : get_short_pointer_();
        }

        /// 短字符串参数
        void set_short_size_(size_type n) {
            rep_.sstr.size = n;
            rep_.sstr.is_long = 0;
        }

        size_type get_short_size_() const {
            assert(rep_.sstr.is_long == 0);
            return rep_.sstr.size;
        }

        /// 获取短字符串在栈中的实际内存地址
        pointer get_short_pointer_() {
            assert(rep_.sstr.is_long == 0);
            return std::addressof(rep_.sstr.data[0]);
        }

        const_pointer get_short_pointer_() const {
            assert(rep_.sstr.is_long == 0);
            return std::addressof(rep_.sstr.data[0]);
        }

        /// 长字符串操作--->获取相关参数
        void set_long_size_(size_type n) {
            rep_.lstr.size = n;
            rep_.lstr.is_long = 1;
        }

        size_type get_long_size_() const {
            assert(rep_.lstr.is_long == 1);
            return rep_.lstr.size;
        }

        void set_long_capacity_(size_type n) {
            rep_.lstr.cap = n - 1;
            rep_.lstr.is_long = 1;
        }

        size_type get_long_capacity_() const {
            assert(rep_.lstr.is_long == 1);
            return rep_.lstr.cap;
        }

        /// 设定相关指针
        void set_long_pointer_(pointer p) {
            rep_.lstr.data = p;
        }

        /// 获取长字符串在堆中的地址
        pointer get_long_pointer_() {
            assert(rep_.lstr.is_long == 1);
            return rep_.lstr.data;
        }

        const_pointer get_long_pointer_() const {
            assert(rep_.lstr.is_long == 1);
            return rep_.lstr.data;
        }

        /// 将数字 n 向上对齐到 N 的整数倍
        /// 对齐到 8 字节，能使结构体、内存块等满足 SIMD/CPU cache 的需求
        template<size_type N>
        static size_type align_it_(size_type n) {
            return (n + N - 1) & ~(N - 1);
        }

        /// 推荐一个“合理的”扩容大小，满足至少为 size，并向上按 8 字节对齐
        /// 类比 STL vector 的 _M_get_new_capacity()
        /// 可以限制最大容量，防止内存爆炸式增长
        static size_type recommend_(size_type size) {
            if (size < kShortCap) {
                return static_cast<size_type>(kShortCap);
            }
            /// (size + 1) 向上对齐到 8 的倍数
            const size_type guess = align_it_<8>(size + 1);
            return guess;
        }

        /// 初始化
        void init_(size_type n, value_type c) {
            pointer p;
            if (fits_in_sso_(n)) {
                p = get_short_pointer_();
                set_short_size_(n);
            } else {
                auto cap = recommend_(n);
                p = alloc_.allocate(cap);
                set_long_capacity_(cap);
                set_long_pointer_(p);
                set_long_size_(n);
            }
            for (size_type i = 0; i < n; ++i) {
                p[i] = c;
            }
            p[n] = value_type();
        }

        void init_( const value_type *str, size_type n) {
            rep_.sstr.is_long = 0;
            pointer p;
            /// 首先判断是否为短字符
            if (fits_in_sso_(n)) {
                p = get_short_pointer_();
                set_short_size_(n);
            } else {
                auto cap = recommend_(n);
                p = alloc_.allocate(cap);
                set_long_capacity_(cap);
                set_long_pointer_(p);
                set_long_size_(n);
            }
            /// 将str的内容拷贝到p上
            memcpy(p, str, n);
            p[n] = value_type();
        }

        /// 内存增长（扩容）策略函数
        /// grow_by_and_add_（） 在旧数据基础上扩容，并在末尾追加新的数据段
        void grow_by_and_add_(size_type old_cap,
                              size_type extra_cap,
                              size_type old_sz,
                              size_type n_add,
                              const value_type *new_buffer) {
            /// 首先调用 recommend_() 函数计算新容量（2倍扩容或满足新增需求）
            auto new_cap = recommend_(my::max(old_cap + extra_cap, 2 * old_cap));
            auto new_data = alloc_.allocate(new_cap);
            if (old_sz > 0) {
                /// 分配成功之后拷贝原数据（可能是 small 模式或 heap 模式）到新分配区
                memcpy(new_data, get_pointer_(), old_sz);
            }
            if (n_add > 0) {
                memcpy(new_data + old_sz, new_buffer, n_add);
            }
            if (is_long_()) {
                alloc_.deallocate(get_long_pointer_(), old_cap);
            }
            /// 更新参数
            set_long_pointer_(new_data);
            set_long_capacity_(new_cap);
            set_long_size_(old_sz + n_add);
            new_data[get_long_size_()] = value_type();
        }

        /// 只扩容，不添加新数据。-->准备插入或 reserve
        void grow_by_(size_type old_cap, size_type extra_cap, size_type old_sz) {
            auto new_cap = recommend_(my::max(old_cap + extra_cap, 2 * old_cap));
            auto new_data = alloc_.allocate(new_cap);
            if (old_sz > 0) {
                memcpy(new_data, get_long_pointer_(), old_sz);
            }
            alloc_.deallocate(get_long_pointer_(), old_cap);
            set_long_pointer_(new_data);
            set_long_capacity_(new_cap);
        }

        /// 容量调整方法---
        /// 根据目标容量 target_capacity，
        /// 在 short buffer（SSO）和 long buffer（堆分配）之间切换，并复制旧数据到新空间中，确保字符串有效
        void shrink_or_extend_(size_type target_capacity) {
            size_type cap = capacity();
            size_type sz = size();

            bool was_long = is_long_();
            bool now_long = true;
            pointer new_data = nullptr;
            pointer old_data = nullptr;
            if (was_long) {
                old_data = get_long_pointer_();
            } else {
                old_data = get_short_pointer_();
            }
            if (fits_in_sso_(target_capacity)) {
                now_long = false;
                new_data = get_short_pointer_();
            } else {
                new_data = alloc_.allocate(target_capacity);
            }
            memcpy(new_data, old_data, sz);
            new_data[sz] = '\0';
            if (was_long) {
                alloc_.deallocate(old_data, cap);
            }
            if (now_long) {
                set_long_pointer_(new_data);
                set_long_capacity_(target_capacity);
                set_long_size_(sz);
            } else {
                set_short_size_(sz);
            }
        }

        /// 赋值短字符串内容到内部缓冲区
        /// 将外部字符串 str 中的前 n 个字符 赋值到 short buffer 中
        basic_string &assign_short_(const value_type *str, size_type n) {
            set_size_(n);
            pointer p = get_pointer_();
            memcpy(p, str, n);
            p[n] = value_type();
            return *this;
        }

        /// 将 str[0..n-1] 写入当前字符串对象的堆空间中。如果容量不够，则：
        /// 调用 grow_by_and_add_() 开辟新堆空间并填入。
        basic_string &assign_long_(const value_type *str, size_type n) {
            size_type cap = capacity();
            if (cap >= n) {
                pointer p = get_long_pointer_();
                memcpy(p, str, n);
                p[n] = value_type();
                set_long_size_(n);
            } else {
                grow_by_and_add_(cap, n - cap, 0, n, str);
            }
            return *this;
        }

        /// 预分配空间
        void reserve(size_type request_capacity) {
            if (request_capacity <= capacity()) {
                return;
            }
            size_type target_capacity = std::max(request_capacity, size());
            target_capacity = recommend_(target_capacity);
            if (target_capacity == capacity()) {
                return;
            }
            shrink_or_extend_(target_capacity);
        }

        /// 自动调整长度
        void shrink_to_fit() {
            size_type target_capacity = recommend_(size());
            if (target_capacity == capacity()) {
                return;
            }
            /// 在栈空间和堆空间中来回切换
            shrink_or_extend_(target_capacity);
        }
    };

    using string = basic_string<char>;

    /// 重载==  ！=
    bool operator==(const my::string& lhs, const my::string& rhs) {
        return lhs.size() == rhs.size() && strcmp(lhs.data(), rhs.data()) == 0;
    }

    bool operator==(const my::string& lhs, const char* rhs) {
        return strcmp(lhs.data(), rhs) == 0;
    }

    bool operator==(const char* lhs, const my::string& rhs) {
        return rhs == lhs;
    }

    bool operator!=(const my::string& lhs, const my::string& rhs) {
        return !(lhs == rhs);
    }

    bool operator!=(const my::string& lhs, const char* rhs) {
        return !(lhs == rhs);
    }

    bool operator!=(const char* lhs, const my::string& rhs) {
        return !(lhs == rhs);
    }

}