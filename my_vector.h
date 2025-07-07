/**
 * @file      my_vector.h
 * @brief     [复刻std:vector]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

#include "memory_/my_allocator.h"
#include "my_utility.h"
#include "my_uninitialized.h"
#include "my_exception.h"
#include "my_algorithm.h"
#include "my_iterator.h"

#include <initializer_list>
#include <type_traits>
#include <memory>

namespace my {

    template<typename T, typename Alloc = MyAlloc<T>>
    class vector {
    public:
        using value_type = T;
        using allocator_type = Alloc;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = typename Alloc::pointer;
        using const_pointer = typename Alloc::const_pointer;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = my::reverse_iterator<iterator>;
        using const_reverse_iterator = my::reverse_iterator<const_iterator>;

        /// 定义成员变量
        pointer start_;         /// 指向开头
        pointer finish_;        /// 指向最后一个有效元素的下一位
        pointer end_of_storage_;    /// 指向分配空间的最后
        Alloc alloc_;

        vector() : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}

        explicit vector(size_type n) {
            create_storage_(n);
            default_initialize_(n);
        }

        /// 构造n个value
        vector(size_type n, const T &value) : vector() {
            /// 都去调用底层实现
            create_storage_(n);
            fill_initialize_(n, value);
        }

        /// initializer_list 构造
        vector(std::initializer_list<T> ilist) {
            create_storage_(ilist.size());
            finish_ = uninitialized_copy_a(ilist.begin(), ilist.end(), start_, alloc_);
        }

        /// initializer_list 赋值
        vector &operator=(std::initializer_list<T> ilist) {
            assign(ilist.begin(), ilist.end());
            return *this;
        }

        /// 范围 assign
        template<typename InputIt>
        void assign(InputIt first, InputIt last) {
            size_type n = my::distance(first, last);
            if (n > capacity()) {
                vector temp(first, last);
                swap(temp);
            } else {
                size_type i = 0;
                for (; first != last && i < size(); ++first, ++i) {
                    start_[i] = *first;
                }
                if (first != last) {
                    finish_ = uninitialized_copy_a(first, last, start_ + i, alloc_);
                } else {
                    erase_at_end_(start_ + i);
                }
            }
        }

        /// 范围构造器
        template<typename InputIt>
        vector(InputIt first, InputIt last) {
            size_type n = std::distance(first, last);
            create_storage_(n);
            finish_ = uninitialized_copy_a(first, last, start_, alloc_);
        }


        /// 拷贝构造
        vector(const vector &other) {
            create_storage_(other.size());
            finish_ = uninitialized_copy_a(other.start_, other.finish_, start_, alloc_);
        }

        /// 移动构造
        vector(vector &&other) noexcept
                : start_(other.start_), finish_(other.finish_), end_of_storage_(other.end_of_storage_) {
            other.start_ = other.finish_ = other.end_of_storage_ = nullptr;
        }

        ~vector() {
            destroy_(start_, finish_, alloc_);
            destroy_storage_();
        }

        /// 迭代器操作
        iterator begin() {
            return start_;
        }

        const_iterator begin() const {
            return start_;
        }

        const_iterator cbegin() const {
            return const_iterator(start_);
        }

        iterator end() {
            return finish_;
        }

        const_iterator end() const {
            return finish_;
        }

        const_iterator cend() const {
            return const_iterator(finish_);
        }

        reverse_iterator rbegin() {
            return reverse_iterator(finish_);
        }

        reverse_iterator rend() {
            return reverse_iterator(start_);
        }

        const_reverse_iterator rbegin() const {
            return reverse_iterator(finish_);
        }

        const_reverse_iterator rend() const {
            return reverse_iterator(start_);
        }

        /// 接口方法
        size_type size() const { return finish_ - start_; }

        size_type capacity() const { return end_of_storage_ - start_; }

        bool empty() const { return start_ == finish_; }

        size_type max_size() const { return alloc_.max_size(); }

        /// 重载[]
        reference operator[](size_type n) {
            return *(start_ + n);
        }

        const_reference operator[](size_type n) const {
            return *(start_ + n);
        }

        /// at()带边界检查
        reference at(size_type n) {
            if (n >= size()) {
                throw my::exception("my::vector<T> out of range");
            }
            return *(start_ + n);
        }

        const_reference at(size_type n) const {
            if (n >= size()) {
                throw my::exception("my::vector<T> out of range");
            }
            return *(start_ + n);
        }

        reference front() {
            return *start_;
        }

        const_reference front() const {
            return *start_;
        }

        reference back() {
            if (finish_ == start_) {
                throw my::exception("my::vector<T> empty");
            }
            return *(finish_ - 1);
        }

        const_reference back() const {
            if (finish_ == start_) {
                throw my::exception("my::vector<T> empty");
            }
            return *(finish_ - 1);
        }

        /// reserve(n)--->涉及到内存操作
        void reserve(size_type n) {
            /// 边界检查
            if (n > max_size()) {
                throw my::exception("my::vector<T> too long");
            }

            if (n > capacity()) {
                size_type old_size = size();
                pointer temp = allocate_and_copy_(n, start_, finish_);
                destroy_(start_, finish_, alloc_);      /// 析构旧的对象
                destroy_storage_();                                 /// 释放旧的空间
                start_ = temp;
                finish_ = start_ + old_size;
                end_of_storage_ = start_ + n;
            }
        }

        void resize(size_type new_size, const value_type &value) {
            /// 判断new_size大小
            if (new_size > size()) {
                fill_insert_(end(), new_size - size(), value);
            } else if (new_size < size()) {
                /// 将末尾size() - new_size个元素删除
                erase_at_end_(start_ + new_size);
            }
        }

        void resize(size_type n) {
            resize(n, T());
        }

        ///------------------insert()
        void insert(iterator pos, const value_type &value) {
            fill_insert_(pos, 1, value);    /// 在指定位置插入一个value
        }

        void insert(iterator pos, size_type n, const value_type &value) {
            fill_insert_(pos, n, value);    /// 在指定位置插入n个value
        }

        /// 添加范围插入
        template<typename InputIt>
        void insert(iterator pos, InputIt first, InputIt last) {
            size_type n = my::distance(first, last);
            if (n == 0) {
                return;
            }

            if (size_type(end_of_storage_ - finish_) >= n) {
                pointer old_finish = finish_;
                finish_ = uninitialized_copy_a(old_finish - n, old_finish, old_finish, alloc_);
                std::copy_backward(pos, old_finish, finish_);
                std::copy(first, last, pos);
            } else {
                size_type old_size = size();
                size_type elems_before = pos - begin();
                size_type new_size = old_size + my::max(old_size, n);
                pointer new_start = alloc_.allocate(new_size);
                pointer new_finish = new_start;

                try {
                    new_finish = uninitialized_copy_a(start_, pos, new_start, alloc_);
                    new_finish = uninitialized_copy_a(first, last, new_finish, alloc_);
                    new_finish = uninitialized_copy_a(pos, finish_, new_finish, alloc_);
                } catch (...) {
                    destroy_(new_start, new_finish, alloc_);
                    alloc_.deallocate(new_start, new_size);
                    throw;
                }
                destroy_(start_, finish_, alloc_);
                destroy_storage_();
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = new_start + new_size;
            }
        }


        void push_back(const value_type &value) {
            insert(end(), value);
        }

        void pop_back() {
            if (start_ == finish_) {
                throw my::exception("my::vector<T> empty");
            }
            --finish_;
            alloc_.destroy(finish_);
        }

        /// 清除所有元素
        void clear() {
            erase_at_end_(start_);
        }

        iterator erase(iterator pos) {
            if (pos + 1 != finish_) {
                /// 将pos后的元素都往前挪
                std::copy(pos + 1, finish_, pos);
            }
            --finish_;
            alloc_.destroy(finish_);
            return pos;
        }

        /// emplace_back模板
        template<typename ...Args>
        void emplace_back(Args &&...args) {
            if (finish_ != end_of_storage_) {
                /// 分配的空间还没有用完，直接进行构造
                std::allocator_traits<Alloc>::construct(alloc_, finish_, my::forward<Args>(args)...);
                ++finish_;
            } else {
                realloc_insert_(end(), my::forward<Args>(args)...);
            }
        }

        /// 支持 emplace 插入
        template<typename... Args>
        iterator emplace(const_iterator pos, Args &&... args) {
            size_type index = pos - cbegin();
            emplace_impl_(begin() + index, std::forward<Args>(args)...);
            return begin() + index;
        }

        pointer data() {
            return start_;
        }

        const_pointer data() const {
            return start_;
        }

        void assign(size_type n, const value_type &value) {
            fill_assign_(n, value);
        }

        void swap(vector &other) {
            my::swap(start_, other.start_);
            my::swap(finish_, other.finish_);
            my::swap(end_of_storage_, other.end_of_storage_);
        }

        /// 自动调整容量大小
        void shrink_to_fit() {
            if (size() < capacity()) {
                size_type old_size = size();
                pointer temp = allocate_and_copy_(old_size, start_, finish_);
                destroy_(start_, finish_, alloc_);
                destroy_storage_();
                start_ = temp;
                finish_ = start_ + old_size;
                end_of_storage_ = finish_;
            }
        }

        /// 重载==
        bool operator==(const vector &other) const {
            if (size() != other.size()) {
                return false;
            }

            /// 如果要相等必须其中的每一个元素都要相等，所以需要循环比较
            for (size_type i = 0; i < size(); ++i) {
                if (start_[i] != other.start_[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const vector &other) const {
            return !(*this == other);
        }

        /// 重载=号
        vector &operator=(const vector &other) {
            /// 自赋值检查
            if (this != &other) {
                const size_type other_size = other.size();
                if (other_size > capacity()) {
                    /// 用来赋值的数组的数量大于当前被赋值数组的长度,需要新开辟空间
                    pointer temp = allocate_and_copy_(other_size, other.start_, other.finish_);
                    destroy_(start_, finish_, alloc_);
                    destroy_storage_();
                    start_ = temp;
                    end_of_storage_ = start_ + other_size;
                } else if (size() >= other.size()) {
                    /// 将赋值的数组拷贝到被赋值的数组的对应位置
                    pointer new_finish = std::copy(other.start_, other.finish_, start_);
                    /// 析构对于的对象
                    destroy_(new_finish, finish_, alloc_);
                } else {
                    std::copy(other.start_, other.start_ + size(), start_);
                    uninitialized_copy_a(other.start_ + size(), other.finish_, finish_, alloc_);
                }
                finish_ = start_ + other_size;
            }
            return *this;
        }

        vector &operator=(vector &&other) noexcept {
            if (this != &other) {
                destroy_(start_, finish_, alloc_);
                destroy_storage_();

                /// 更新成员变量
                start_ = other.start_;
                finish_ = other.finish_;
                end_of_storage_ = other.end_of_storage_;
                /// 将other的成员变量都置为nullptr
                other.start_ = other.finish_ = other.end_of_storage_ = nullptr;
            }
            return *this;
        }

    private:
        /// emplace 的底层实现，内部使用 move_if_noexcept
        template<typename... Args>
        void emplace_impl_(iterator pos, Args &&... args) {
            if (finish_ != end_of_storage_) {
                std::move_backward(pos, finish_, finish_ + 1);
                std::allocator_traits<Alloc>::construct(alloc_, pos, std::forward<Args>(args)...);
                ++finish_;
            } else {
                realloc_emplace_(pos, std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        void realloc_emplace_(iterator pos, Args &&... args) {
            size_type old_size = size();
            size_type elems_before = pos - start_;
            size_type new_size = old_size + my::max(old_size, size_type(1));
            pointer new_start = alloc_.allocate(new_size);
            pointer new_finish = new_start;

            try {
                new_finish = uninitialized_move_a(start_, pos, new_start, alloc_);
                std::allocator_traits<Alloc>::construct(alloc_, new_start + elems_before, my::forward<Args>(args)...);
                ++new_finish;
                new_finish = uninitialized_move_a(pos, finish_, new_start + elems_before + 1, alloc_);
            } catch (...) {
                destroy_(new_start, new_finish, alloc_);
                alloc_.deallocate(new_start, new_size);
                throw;
            }

            destroy_(start_, finish_, alloc_);
            destroy_storage_();
            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + new_size;
        }

        void create_storage_(size_type n) {
            /// 分配空间
            start_ = alloc_.allocate(n);
            finish_ = start_;
            end_of_storage_ = start_ + n;
        }

        void destroy_storage_() {
            /// 释放空间
            alloc_.deallocate(start_, end_of_storage_ - start_);
            /// 将指针全部置为nullptr
            start_ = finish_ = end_of_storage_ = nullptr;
        }

        void fill_initialize_(size_type n, const T &value) {
            finish_ = uninitialized_fill_n_a(start_, n, value, alloc_); /// 调用底层的分配方法
        }

        void default_initialize_(size_type n) {
            finish_ = uninitalized_default_n_a(start_, n, alloc_);
        }

        void erase_at_end_(pointer pos) {
            if (finish_ > pos) {
                destroy_(pos, finish_, alloc_);
                finish_ = pos;
            }
        }

        template<typename ForwordIterator>
        pointer allocate_and_copy_(size_type n, ForwordIterator first, ForwordIterator last) {
            pointer result = alloc_.allocate(n);
            try {
                uninitialized_copy_a(first, last, result, alloc_);
                return result;
            } catch (...) {
                alloc_.deallocate(result, n);
                throw;
            }
        }

        /// realloc_insert()
        template <typename... Args>
        void realloc_insert_(iterator pos, Args&&... args) {
            const size_type old_size = size();
            const size_type elems_before = pos - start_;
            const size_type new_capacity = old_size + my::max(old_size, size_type(1));  // grow exponentially

            pointer new_start = alloc_.allocate(new_capacity);
            pointer new_finish = new_start;

            try {
                new_finish = uninitialized_copy_a(start_, pos, new_start, alloc_);

                std::allocator_traits<Alloc>::construct(alloc_, new_start + elems_before, std::forward<Args>(args)...);

                new_finish = new_start + elems_before + 1;

                new_finish = uninitialized_copy_a(pos, finish_, new_finish, alloc_);
            } catch (...) {
                destroy_(new_start, new_finish, alloc_);
                alloc_.deallocate(new_start, new_capacity);
                throw;
            }

            destroy_(start_, finish_, alloc_);
            destroy_storage_();

            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + new_capacity;
        }



        /// 在指定范围内插入
        void fill_insert_(iterator pos, size_type n, const value_type &value) {
            if (n == 0) {
                return;
            }

            if (size_type(end_of_storage_ - finish_) > n) {
                /// 后面还有足够的空间容纳n个value
                const size_type elems_after = finish_ - pos;
                pointer old_finish = finish_;
                if (elems_after > n) {
                    /// 如果pos后的元素比n多，则往后挪n个位置，给pos处开始留n个空间value
                    finish_ = uninitialized_copy_a(finish_ - n, finish_, finish_, alloc_);
                    std::fill_n(pos, n, value);
                } else {
                    finish_ = uninitialized_fill_n_a(finish_, n - elems_after, value, alloc_);
                    /// 现在是使用的拷贝，后期优化成move
                    finish_ = uninitialized_copy_a(pos, old_finish, finish_, alloc_);
                    std::fill(pos, old_finish, value);
                }
            } else {
                /// 如果后面的空间不够了，需要开辟新的空间
                const size_type old_size = size();
                const size_type elems_before = pos - begin();
                const size_type new_size = old_size + my::max(old_size, n);
                pointer new_start = alloc_.allocate(new_size);
                pointer new_finish = new_start;

                try {
                    uninitialized_fill_n_a(new_start + elems_before, n, value, alloc_);
                    /// 这里先使用copy后期优化成move()
                    new_finish = uninitialized_copy_a(start_, pos, new_start, alloc_);
                    new_finish += n;
                    new_finish = uninitialized_copy_a(pos, finish_, new_finish, alloc_);
                } catch (...) {
                    if (!new_finish) {
                        destroy_(new_start + elems_before, new_start + elems_before + n, alloc_);
                    } else {
                        destroy_(new_start, new_finish, alloc_);
                    }
                    alloc_.deallocate(new_start, new_size);
                    throw;
                }

                destroy_(start_, finish_, alloc_);
                destroy_storage_();
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = start_ + new_size;
            }
        }

        void fill_assign_(size_type n, const value_type &value) {
            if (n > capacity()) {
                vector temp(n, value);
                this->swap(temp);
            } else if (n > size()) {
                std::fill(start_, finish_, value);
                finish_ = uninitialized_fill_n_a(finish_, n - size(), value, alloc_);
            } else if (n < size()) {
                std::fill(start_, start_ + n, value);
                erase_at_end_(start_ + n);

            } else {
                erase_at_end_(start_ + n);
            }
        }
    };

}