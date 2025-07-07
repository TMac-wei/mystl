/**
 * @file      my_deque.h
 * @brief     [deque复现]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#pragma once

#include "my_uninitialized.h"
#include "my_iterator.h"

#include <cassert>

namespace my {
    /// 确定每个节点存放的大小
    /// 典型的 deque 实现中，数据分散在多个连续的“缓冲区”（chunk）中，而这些 chunk 被指针数组（map）管理
    template <typename T>
    constexpr size_t default_chunk_size = (sizeof(T) < 256) ? (4096 / sizeof(T)) : 16;

    /// deque_iterator
    template <typename T, typename Ref, typename Ptr>
    struct deque_iterator {
        using iterator = deque_iterator<T, T&, T*>;
        using const_iterator = deque_iterator<T,const T&,const T*>;
        using self = deque_iterator;

        using elt_pointer = T*;         /// 表明节点里面的位置指针
        using map_pointer = T**;        /// 这是节点数组的指针

        using value_type = T;
        using pointer = Ptr;
        using reference = Ref;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;      // 随机访问迭代器

        /// 定义节点里面的指针----->这四个是成员变量
        map_pointer node_;              /// 节点指针
        elt_pointer first_;             /// 节点里面的第一个位置
        elt_pointer cur_;               /// 节点中现在处于哪个位置
        elt_pointer last_;              /// 最后的位置的下一个位置（迭代器末尾一般指向最后一个元素的下一个位置）

        static constexpr size_t chunk_size_ = default_chunk_size<T>;
//        size_t chunk_size_;
        /// 构造函数操作
        deque_iterator()
                : node_(nullptr), first_(nullptr), cur_(nullptr), last_(nullptr) {}

        deque_iterator(elt_pointer cur, map_pointer node)
                : node_(node), first_(*node), cur_(cur), last_(*node + default_chunk_size<T>){}

//        deque_iterator(elt_pointer cur, map_pointer node)
//                : node_(node), first_(*node), cur_(cur), last_(*node + default_chunk_size<T>) {}


        template <typename T2, typename Ref2, typename Ptr2>
        deque_iterator(const deque_iterator<T2, Ref2, Ptr2>& x)
                : node_(x.node_), first_(x.first_), cur_(x.cur_), last_(x.last_){}

        /// 解引用和指针操作
        reference operator*() const
        {
            return *cur_;
        }

        pointer operator->() const
        {
            return cur_;
        }

        /// 迭代器的前/后置 ++/--
        self operator++()
        {
            ++cur_;
            /// 必须要判断指针+1之后是否需要切换节点
            if (cur_ == last_) {
                set_node_(node_ + 1);        /// 切换到下一个节点
                cur_ = first_;

            }
            return *this;
        }

        /// 后置++
        self operator++(int)
        {
            self tem = *this;
            ++(*this);
            return tem;
        }

        self operator--()
        {
            /// 必须要判断指针+1之后是否需要切换节点
            if (cur_ == first_) {
                set_node_(node_ - 1);        /// 切换到下一个节点
                cur_ = last_;
            }
            --cur_;
            return *this;
        }

        self operator--(int)
        {
            self tem = *this;
            --(*this);
            return tem;
        }

        /// operator +=是核心操作
        self& operator+=(difference_type n)
        {
            /**
             * 1、首先计算当前位置与当前缓冲区中的起始位置的偏移
             * 2、对偏移量进行判断
             *     2.1 若是同一个Node，直接改动偏移量
             *     2.2 不是在同一个Node
             *          2.2.1 计算移动的节点，移动到新位置
             *          2.2.2 offset是负数，向前移动
             * */
            difference_type offset = static_cast<difference_type>(cur_ - first_) + n;
            /// 判断是否在同一个缓冲区中
            if (offset >= 0 && offset < static_cast<difference_type>(chunk_size_)) {
                /// 在统一个缓冲Node中，不需要跨Node移动
                cur_ += n;
            } else {
                /// 不在同一个Node中，需要跨Node移动
                /// 计算移动的节点
                difference_type node_offset = offset > 0
                                              ? offset / static_cast<difference_type>(chunk_size_)
                                              : -((-offset - 1) / static_cast<difference_type>(chunk_size_)) - 1;
                /// 移动到新的节点
                set_node_(node_offset + node_);
                /// 移动到新的位置  总的偏移的位置-移动了几个节点
                cur_ = first_ + (offset - node_offset * static_cast<difference_type>(chunk_size_));
            }
            return *this;
        }

        /// 后续的 + - -=都是基于+=
        self operator+(difference_type n) const
        {
            self temp = *this;
            return temp += n;
        }

        self& operator-=(difference_type n)
        {
            return *this += (-n);
        }

        self operator-(difference_type n) const
        {
            self temp = *this;
            return temp -= n;
        }

        /// 重载[]
        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }

        bool operator==(const self& x) const
        {
            return cur_ == x.cur_;
        }

        bool operator!=(const self& x) const
        {
            return cur_ != x.cur_;
            /// return *this != x;  这是无限递归使用自己
        }

        bool operator<(const self& x) const
        {
            /// 先看是不是一个节点，如果是一个节点，则比较cur_指针的大小，如果不是一个节点就比较节点
            return (node_ == x.node_) ? (cur_ < x.cur_) : (node_ < x.node_);
        }

        bool operator>(const self& x) const
        {
            /// 先看是不是一个节点，如果是一个节点，则比较cur_指针的大小，如果不是一个节点就比较节点
            return x < *this;
        }

        bool operator<=(const self& x) const
        {
            return !(x < *this);
        }

        bool operator>=(const self& x) const
        {
            return !(*this < x);
        }


        /**
           * 两个迭代器之间的距离(*this 迭代器和 x 迭代器之间横跨多个缓冲区的总距离（单位是元素数）)
           * 适用于像 STL deque 这样基于分段缓冲区设计的数据结构。由于缓冲区之间不是连续内存，
           * 因此必须考虑完整块的数量 + 每端的偏移量
           *
           * 返回值：当前迭代器 *this 与参数迭代器 x 之间的元素距离
           * 假设：
           *      *this 是较大的迭代器（指向靠后的元素）
           *      x 是较小的迭代器
           * 结果：
           *      一般为每个缓冲区的大小--->（deque_buf_size_())
           *      当前迭代器与参数迭代器之间相隔的完整缓冲区数量、
           *          不含头尾 -1是因为头缓冲区不完全用完，需要单独计算  ---> (node_ - x.node_ - 1)
           *
           *      difference_type(deque_buf_size_()) * (node_ - x.node_ - 1)
                      计算完整缓冲区中包含的元素个数
                  (cur_ - first_)
                      当前迭代器（*this）在当前缓冲区中的偏移量（相对 first_）
                  (x.last_ - x.cur_)
                      被减的迭代器 x 在当前缓冲区中剩余的元素个数（从 x.cur_ 到缓冲区末尾 x.last_）
           * */
        difference_type operator-(const self& x) const
        {
//            std::cout << "chunk_size_ = " << chunk_size_ << "\n";
            return static_cast<difference_type>(chunk_size_) * (node_ - x.node_) + (cur_ - first_) - (x.cur_ - x.first_);
        }

        /// 设置当前是在哪一个Node上
        void set_node_(map_pointer node)
        {
            /// 需要进行边界检查
///        assert(node && *node);
            node_ = node;
            first_ = *node;
            cur_ = first_;          /// 保证cur在新块同位置
            last_ = first_ + chunk_size_;
            assert(chunk_size_ > 0 && "chunk_size_ must not be zero!");
        }

        /// 验证 deque 迭代器（或节点）的内部状态是否符合预期逻辑 这个函数可以不要！
        void check_invariant() const {
            assert(first_ && last_ && "chunk pointers must not be null");
            assert(cur_ >= first_ && cur_ <= last_ && "cur_ must be within [first_, last_)");
        }
    };

    /// deque_base是deque的基类
    template <typename T, typename Alloc = std::allocator<T>>
    class deque_base {
    public:
        using value_type    = T;
        using allocate_type = Alloc;
        using size_type     = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer       = typename std::allocator_traits<Alloc>::pointer;
        using const_pointer       = typename std::allocator_traits<Alloc>::const_pointer;
        using iterator = deque_iterator<T, T&, T*>;
        using const_iterator = deque_iterator<T, const T&, const T*>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static constexpr size_t chunk_size_ = default_chunk_size<T>;

        using map_pointer   = typename iterator::map_pointer ; /// T**

//        using map_allocator = typename Alloc::template rebind<T>::other;
//        using data_allocator = typename Alloc::template rebind<pointer>::other;
        /// 上面的写法是c++11中，但是在c++20已经被放弃了，我的工程建立在c++20，所以修改为下面
        using map_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<T*>;
        using data_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;


        /// 构造函数
        deque_base()
        {
            initialize_map_(0);
        }

        deque_base(size_t num_elements)
        {
            initialize_map_(num_elements);
        }

        ~deque_base()
        {
            destroy_nodes_(start_.node_, finish_.node_ + 1);
            map_allocator_.deallocate(map_, map_size_);
        }

        /// deque自己的swap函数
        void swap_data( deque_base & other)
        {
            std::swap(start_, other.start_);
            std::swap(finish_, other.finish_);
            std::swap(map_, other.map_);
            std::swap(map_size_, other.map_size_);
            std::swap(data_allocator_, other.data_allocator_);
            std::swap(map_allocator_, other.map_allocator_);
        }

    protected:
        /// 成员变量
        iterator start_;
        iterator finish_;
        map_pointer map_;
        size_type map_size_;
        data_allocator data_allocator_;
        map_allocator map_allocator_;

        /// 析构节点对象以及析构节点和分配节点的接口
        void create_nodes_(map_pointer nstart, map_pointer nfinish)
        {
            map_pointer cur;
            try{
                for (cur = nstart; cur < nfinish; cur++) {
                    *cur = allocate_node();
                }
            }catch (...) {
                destroy_nodes_(nstart, cur);
                throw ;
            }
        }

        pointer allocate_node()
        {
            return data_allocator_.allocate(chunk_size_);
        }

        void destroy_nodes_(map_pointer nstart, map_pointer nfinish)
        {
            for (map_pointer n = nstart; n < nfinish; ++n) {
                deallocate_node_(*n);
            }
        }

        void deallocate_node_(pointer n)
        {
            data_allocator_.deallocate(n, chunk_size_);
        }

    private:
        /// 初始化"map"控制器
        void initialize_map_(size_type num_elements)
        {
            /// 计算需要的节点数
            size_type num_nodes = num_elements / chunk_size_ + 1;
            /// 至少分配8个节点，2个额外节点
            map_size_ = std::max(size_type(8), num_nodes + 2);
            map_ = map_allocator_.allocate(map_size_);
            /// 计算起始节点，使得map_的中间位置对齐
            map_pointer nstart = map_ + (map_size_ - num_nodes) / 2;
            map_pointer nfinish = nstart + num_nodes;
            try {
                create_nodes_(nstart, nfinish);
            } catch(...) {
                map_allocator_.deallocate(map_, map_size_);
                map_ = nullptr;
                map_size_ = 0;
                throw;
            }

            start_.set_node_(nstart);
            finish_.set_node_(nfinish - 1);
            start_.cur_ = start_.first_;
            finish_.cur_ = finish_.first_ + num_elements % chunk_size_;
        }
    };

    template <typename T, typename Alloc = std::allocator<T>>
    class deque : protected deque_base<T, Alloc> {
    public:
        using Base = deque_base<T, Alloc>;
        using map_pointer = typename Base::map_pointer;

        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;
        using allocator_type = Alloc;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using const_pointer = const T*;
        using const_reference = const T&;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using Base::allocate_node;
        using Base::data_allocator_;
        using Base::deallocate_node_;
        using Base::destroy_nodes_;
        using Base::finish_;
        using Base::map_;
        using Base::map_allocator_;
        using Base::map_size_;
        using Base::start_;
        using Base::swap_data;

    public:
        deque() : Base() {
            default_initialize_();
        }

        deque(size_type n) : Base(n) {
            fill_initialize_(value_type());
        }

        deque(size_type n, const value_type& value) : Base(n) {
            fill_initialize_(value);
        }

        deque(const deque& other) : Base(other.size()) {
            uninitialized_copy_a(other.begin(), other.end(), start_, data_allocator_);
        }

        deque(deque&& other) noexcept : Base() {
            swap_data(other);
        }

        ~deque() {
            destroy_data_(begin(), end());
        }

        /// 迭代器操作
        iterator begin() {
            return start_;
        }

        const_iterator begin() const {
            return start_;
        }

        iterator end() {
            return finish_;
        }

        const_iterator end() const {
            return finish_;
        }

        reverse_iterator rbegin() {
            return reverse_iterator(finish_);
        }

        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(finish_);
        }

        reverse_iterator rend() {
            return reverse_iterator(start_);
        }

        const_reverse_iterator rend() const {
            return const_reverse_iterator(start_);
        }

        /// 常用接口
        size_type size() const {
            return finish_ - start_;
        }

        /// 返回这个map_能分配的最大大小
        size_type max_size() const {
            return std::allocator_traits<allocator_type>::max_size(data_allocator_);
        }

        /// 以下是关于插入操作
        /// push_front()/back()
        void push_front(const value_type& v) {
            if (start_.cur_ != start_.first_) {
//                data_allocator_.construct(start_.cur_ - 1, v);
                /// 使用c++20支持的最新版本写法
                std::allocator_traits<decltype(data_allocator_)>::construct(data_allocator_, start_.cur_ - 1, v);
                --start_.cur_;
            } else {
                push_front_aux_(v);
            }
        }

        void push_back(const value_type& v) {
            if (finish_.cur_ != finish_.last_ - 1) {
//                data_allocator_.construct(finish_.cur_, v);
                /// 改用c++20支持的最新写法
                std::allocator_traits<decltype(data_allocator_)>::construct(data_allocator_, finish_.cur_, v);
                ++finish_.cur_;
            } else {
                push_back_aux_(v);
            }
        }

        /// emplace_back()/front()
        /// emplace()是直接在已经分配的内存上进行构造，支持可变参模板
        template <typename... Args>
        void emplace_front(Args&&... args) {
            if (start_.cur_ != start_.first_) {
//                data_allocator_.construct(start_.cur_ - 1, std::forward<Args>(args)...);
                std::allocator_traits<decltype(data_allocator_)>::construct(
                        data_allocator_, start_.cur_ - 1, std::forward<Args>(args)...);
                --start_.cur_;
            } else {
                push_front_aux_(std::forward<Args>(args)...);
            }
        }

        template <typename... Args>
        void emplace_back(Args&&... args) {
            if (finish_.cur_ != finish_.last_ - 1) {
//                data_allocator_.construct(finish_.cur_, std::forward<Args>(args)...);
                std::allocator_traits<decltype(data_allocator_)>::construct(
                        data_allocator_, finish_.cur_, std::forward<Args>(args)...);
                ++finish_.cur_;
            } else {
                push_back_aux_(std::forward<Args>(args)...);
            }
        }

        /// insert()
        iterator insert(iterator pos, const value_type& v) {
            if (pos.cur_ == start_.cur_) {
                push_front(v);
                return start_;
            } else if (pos.cur_ == finish_.cur_) {
                push_back(v);
                iterator tmp = finish_;
                --tmp;
                return tmp;
            } else {
                return insert_aux_(pos, v);
            }
        }

        iterator insert(iterator pos, size_type n, const value_type& v) {
            difference_type off = pos - start_;
            fill_insert_(pos, n, v);
            return start_ + off;
        }

        /// pop
        void pop_front() {
            if (start_.cur_ != start_.last_ - 1) {
//                data_allocator_.destroy(start_.cur_);
                std::allocator_traits<decltype(data_allocator_)>::destroy(data_allocator_, start_.cur_);
                ++start_.cur_;
            } else {
                pop_front_aux_();
            }
        }

        void pop_back() {
            if (finish_.cur_ != finish_.first_) {
                --finish_.cur_;
//                data_allocator_.destroy(finish_.cur_);
                std::allocator_traits<decltype(data_allocator_)>::destroy(data_allocator_, finish_.cur_);
            } else {
                pop_back_aux_();
            }
        }

        reference front() { return *start_; }

        const_reference front() const { return *start_; }

        /// finish_对应的是最后一个有效节点的下一个节点
        reference back() { return *(finish_ - 1); }

        const_reference back() const { return *(finish_ - 1); }

        void resize(size_type new_size) { resize(new_size, value_type()); }

        void resize(size_type new_size, const value_type& value) {
            size_type len = size();
            if (new_size < len) {
                erase_at_end_(start_ + new_size);
            } else if (new_size > len) {
                fill_insert_(end(), new_size - len, value);
            }
        }

        /// 清除所有的元素
        void clear() {
            erase_at_end_(begin());
        }

        bool empty() const {
            return start_ == finish_;
        }

        reference operator[](size_type n) {
            return start_[n];
        }

        const_reference operator[](size_type n) const {
            return start_[n];
        }

        iterator erase(iterator pos) {
            return erase_(pos);
        }

        void swap(deque& other) {
            swap_data(other);
        }

    private:
        static size_t deque_buf_size_() { return Base::chunk_size_; }

        /// 默认初始化，调用底层的uninitialized_default_a去进行初始化
        void default_initialize_() {
            for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
                uninitialized_default_a(*cur, *cur + deque_buf_size_(), data_allocator_);
            }
            uninitialized_default_a(finish_.first_, finish_.cur_, data_allocator_);
        }

        /// 用value填充
        void fill_initialize_(const value_type& value) {
            for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
                uninitialized_fill_a(*cur, *cur + deque_buf_size_(), value, data_allocator_);
            }
            uninitialized_fill_a(finish_.first_, finish_.cur_, value, data_allocator_);
        }

        void destroy_data_(iterator first, iterator last) {
            for (map_pointer cur = first.node_ + 1; cur < last.node_; ++cur) {
                destroy_(*cur, *cur + deque_buf_size_(), data_allocator_);
            }
            if (first.node_ != last.node_) {
                /// first和last不在同一个节点
                destroy_(first.cur_, first.last_, data_allocator_);
                destroy_(last.first_, last.cur_, data_allocator_);
            } else {
                destroy_(first.cur_, last.cur_, data_allocator_);
            }
        }

        void erase_at_end_(iterator pos) {
            destroy_data_(pos, end());
            destroy_nodes_(pos.node_ + 1, finish_.node_ + 1);
            finish_ = pos;
        }

        iterator reserve_elements_at_front_(size_type n) {
            size_type vacancies = start_.cur_ - start_.first_;
            if (n > vacancies) {
                new_elements_at_front_(n - vacancies);
            }
            return start_ - difference_type(n);
        }

        iterator reserve_elements_at_back_(size_type n) {
            size_type vacancies = finish_.last_ - finish_.cur_ - 1;
            if (n > vacancies) {
                new_elements_at_back_(n - vacancies);
            }
            return finish_ + difference_type(n);
        }

        void new_elements_at_front_(size_type new_elems) {
            if (max_size() - size() < new_elems) {
                throw std::length_error("my::deque<T> too long");
            }
            size_type new_nodes = (new_elems + deque_buf_size_() - 1) / deque_buf_size_();
            reserve_map_at_front_(new_nodes);
            size_type i = 1;
            try {
                for (; i <= new_nodes; ++i) {
                    *(start_.node_ - i) = allocate_node();
                }
            } catch (...) {
                for (size_type j = 1; j < i; ++j) {
                    deallocate_node_(*(start_.node_ - j));
                }
                throw;
            }
        }

        void new_elements_at_back_(size_type new_elems) {
            if (max_size() - size() < new_elems) {
                throw std::length_error("my::deque<T> too long");
            }
            size_type new_nodes = (new_elems + deque_buf_size_() - 1) / deque_buf_size_();
            reserve_map_at_back_(new_nodes);
            size_type i = 1;
            try {
                for (; i <= new_nodes; ++i) {
                    *(finish_.node_ + i) = allocate_node();
                }
            } catch (...) {
                for (size_type j = 1; j < i; ++j) {
                    deallocate_node_(*(finish_.node_ + j));
                }
                throw;
            }
        }

        void reserve_map_at_back_(size_type nodes_to_add = 1) {
            if (nodes_to_add + 1 > map_size_ - (finish_.node_ - map_)) {
                reallocate_map_(nodes_to_add, false);
            }
        }

        void reserve_map_at_front_(size_type nodes_to_add = 1) {
            if (nodes_to_add > start_.node_ - map_) {
                reallocate_map_(nodes_to_add, true);
            }
        }

        void reallocate_map_(size_type nodes_to_add, bool add_at_front) {
            size_type old_num_nodes = finish_.node_ - start_.node_ + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_nstart;
            if (map_size_ > 2 * new_num_nodes) {
                new_nstart = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < start_.node_) {  /// 在前面添加
                    /// 为什么要+1，finish_是最后一个元素的下一个位置
                    std::copy(start_.node_, finish_.node_ + 1, new_nstart);
                } else {  /// 在后面添加
                    std::copy_backward(start_.node_, finish_.node_ + 1, new_nstart + old_num_nodes);
                }
            } else {
                size_type new_map_size = map_size_ + std::max(map_size_, nodes_to_add) + 2;
                map_pointer new_map = map_allocator_.allocate(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                std::copy(start_.node_, finish_.node_ + 1, new_nstart);
                map_allocator_.deallocate(map_, map_size_);
                map_ = new_map;
                map_size_ = new_map_size;
            }
            start_.set_node_(new_nstart);
            finish_.set_node_(new_nstart + old_num_nodes - 1);
        }

        template <typename... Args>
        void push_back_aux_(Args&&... args) {
            reserve_map_at_back_();
            *(finish_.node_ + 1) = allocate_node();
            try {
//                data_allocator_.construct(finish_.cur_, std::forward<Args>(args)...);
                std::allocator_traits<decltype(data_allocator_)>::construct(
                        data_allocator_, finish_.cur_, std::forward<Args>(args)...);
                finish_.set_node_(finish_.node_ + 1);
                finish_.cur_ = finish_.first_;
            } catch (...) {
                deallocate_node_(*(finish_.node_ + 1));
                throw;
            }
        }

        template <typename... Args>
        void push_front_aux_(Args&&... args) {
            reserve_map_at_front_();
            *(start_.node_ - 1) = allocate_node();
            try {
                start_.set_node_(start_.node_ - 1);
                start_.cur_ = start_.last_ - 1;
//                data_allocator_.construct(start_.cur_, std::forward<Args>(args)...);
                std::allocator_traits<decltype(data_allocator_)>::construct(
                        data_allocator_, start_.cur_, std::forward<Args>(args)...);
            } catch (...) {
                deallocate_node_(*(start_.node_ - 1));
                throw;
            }
        }

        void pop_back_aux_() {
            deallocate_node_(finish_.first_);
            finish_.set_node_(finish_.node_ - 1);
            finish_.cur_ = finish_.last_ - 1;
//            data_allocator_.destroy(finish_.cur_);
            std::allocator_traits<decltype(data_allocator_)>::destroy(data_allocator_, finish_.cur_);
        }

        void pop_front_aux_() {
//            data_allocator_.destroy(start_.cur_);
            std::allocator_traits<decltype(data_allocator_)>::destroy(data_allocator_, start_.cur_);
            deallocate_node_(start_.first_);
            start_.set_node_(start_.node_ + 1);
            start_.cur_ = start_.first_;
        }

        iterator insert_aux_(iterator pos, const value_type& v) {
            value_type v_copy = v;
            difference_type index = pos - start_;
            if (index < size() / 2) {  /// 在前半部分插入
                push_front(front());
                iterator front1 = start_;
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start_ + index;
                iterator pos1 = pos;
                ++pos1;
                std::copy(front2, pos1, front1);  /// [front2, pos1) -> [front1, pos)
            } else {
                push_back(back());
                iterator back1 = finish_;
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start_ + index;
                std::copy_backward(pos, back2, back1);  /// [pos, back2) -> [pos + 1, back1)
            }
            *pos = std::move(v_copy);
            return pos;
        }

        void insert_aux_(iterator pos, size_type n, const value_type& v) {
            const difference_type elems_before = pos - start_;
            size_type length = size();
            value_type v_copy = v;
            if (elems_before < length / 2) {  /// 在前半部分插入
                iterator new_start = reserve_elements_at_front_(n);
                iterator old_start = start_;
                pos = start_ + elems_before;
                try {
                    if (elems_before >= difference_type(n)) {  /// 前面的元素足够多
                        iterator start_n = start_ + difference_type(n);
                        // [start_, start_n) -> [new_start, new_start + n)
                        uninitialized_copy_a(start_, start_n, new_start, data_allocator_);
                        start_ = new_start;
                        // [start_n, pos) -> [old_start, pos)
                        std::copy(start_n, pos, old_start);
                        // [pos, pos + n) -> [pos, pos + n)
                        std::fill(pos - difference_type(n), pos, v_copy);
                    } else {
                        // [start_, pos) -> [new_start, new_start + elems_before)
                        uninitialized_copy_a(start_, pos, new_start, data_allocator_);
                        uninitialized_fill_n_a(new_start + elems_before, n - elems_before, v_copy, data_allocator_);
                        start_ = new_start;
                        std::fill(old_start, pos, v_copy);
                    }
                } catch (...) {
                    destroy_nodes_(new_start.node_, start_.node_);
                    throw;
                }
            } else {
                iterator new_finish = reserve_elements_at_back_(n);
                iterator old_finish = finish_;
                const difference_type elems_after = length - elems_before;
                pos = finish_ - elems_after;
                try {
                    if (elems_after > difference_type(n)) {  // 后面的元素足够多
                        iterator finish_n = finish_ - difference_type(n);
                        // [finish_ - n, finish_) -> [finish_, finish_ + n)
                        uninitialized_copy_a(finish_n, finish_, finish_, data_allocator_);
                        finish_ = new_finish;
                        // [pos, finish_n) -> [old_finish - n, old_finish)
                        std::copy_backward(pos, finish_n, old_finish);
                        std::fill(pos, pos + difference_type(n), v_copy);
                    } else {
                        uninitialized_fill_n_a(finish_, n - elems_after, v_copy,
                                               data_allocator_);  //
                        uninitialized_copy_a(pos, finish_, finish_ + n, data_allocator_);
                        finish_ = new_finish;
                        std::fill(pos, old_finish, v_copy);
                    }
                } catch (...) {
                    destroy_nodes_(finish_.node_ + 1, new_finish.node_ + 1);
                    throw;
                }
            }
        }

        void fill_insert_(iterator pos, size_type n, const value_type& v) {
            if (pos.cur_ == start_.cur_) {
                iterator new_start = reserve_elements_at_front_(n);
                try {
                    uninitialized_fill_n_a(new_start, n, v, data_allocator_);
                    start_ = new_start;
                } catch (...) {
                    destroy_data_(new_start, start_);
                    throw;
                }
            } else if (pos.cur_ == finish_.cur_) {
                iterator new_finish = reserve_elements_at_back_(n);
                try {
                    uninitialized_fill_n_a(finish_, n, v, data_allocator_);
                    finish_ = new_finish;
                } catch (...) {
                    destroy_data_(finish_, new_finish);
                    throw;
                }
            } else {
                insert_aux_(pos, n, v);
            }
        }

        iterator erase_(iterator pos) {
            iterator next = pos;
            ++next;
            difference_type elems_before = pos - begin();
            if (elems_before < size() / 2) {  // 在前半部分
                if (pos != begin()) {
                    std::copy_backward(begin(), pos, next);
                }
                pop_front();
            } else {
                if (next != end()) {
                    std::copy(next, end(), pos);
                }
                pop_back();
            }
            return begin() + elems_before;
        }



    };










}