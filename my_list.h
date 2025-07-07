/**
 * @file      my_list.h
 * @brief     [my_list复现]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "memory_/my_allocator.h"
#include "my_initializer_list.h"
#include "my_iterator.h"
#include "my_utility.h"

namespace my {
    /// 定义链表节点
    /// list底层是一个双向链表实现
/// 先定义链表的节点结构体
    template<typename T>
    struct ListNode {
        T data_;            /// 节点数据
        ListNode *prev_;     /// 指向前一节点
        ListNode *next_;    /// 指向后一个节点

        ListNode()
                : data_{}, prev_(nullptr), next_(nullptr) {}

        ListNode(const T &val)  /// 不能加默认值，否则会与默认构造函数竞争
                : prev_(nullptr), next_(nullptr), data_(val) {}

        ///为 emplace/back/front 等完美转发构造
        template<typename ...Args>
        ListNode(Args &&... args)
                : prev_(nullptr), next_(nullptr), data_(std::forward<Args>(args)...) {}
    };

    template<typename T>
    class ListIterator {
    public:
        /// 双向迭代器
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        ListIterator() : node_(nullptr) {}

        explicit ListIterator(ListNode<T> *node)
                : node_(node) {}

        /// 这个函数模板允许从 ListIterator<U> 转换为 ListIterator<T>（如 T = const string, U = string）
        template<typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
        explicit ListIterator(ListNode<U> *node)
                : node_(reinterpret_cast<ListNode<T> *>(node)) {}

        /// 解引用
        reference operator*() const {
            return node_->data_;
        }

        pointer operator->() const {
            return &(operator*());
        }

        /// 迭代器的前后置++/--
        ListIterator &operator++() {
            node_ = node_->next_;
            return *this;
        }

        ListIterator &operator--() {
            node_ = node_->prev_;
            return *this;
        }

        ListIterator operator++(int) {
            ListIterator temp = *this;
            ++*this;
            return temp;
        }

        ListIterator operator--(int) {
            ListIterator temp = *this;
            --*this;
            return temp;
        }

        /// 重载==  ！=
        bool operator==(const ListIterator &other) const {
            return node_ == other.node_;
        }

        bool operator!=(const ListIterator &other) const {
            return !(*this == other);
        }

        template<typename, typename> friend
        class list;

    private:
        ListNode<T> *node_;
    };

    /// == ！=
    template<typename T>
    bool operator==(const ListIterator<T> &lhs, const ListIterator<T> &rhs) {
        return lhs.operator==(rhs);
    }

    template<typename T>
    bool operator!=(const ListIterator<T> &lhs, const ListIterator<T> &rhs) {
        return lhs.operator!=(rhs);
    }

    /// 以下是list标准实现
    template<typename T, typename Alloc = MyAlloc<T>>
    class list {
    public:
        using list_node = ListNode<T>;
        using link_type = list_node *;

        using value_type = T;
//    using allocator_type                  =     typename Alloc::template rebind<list_node >::other;
        using allocator_type = typename std::allocator_traits<Alloc>::template rebind_alloc<list_node>;

        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;
        using iterator = ListIterator<T>;
        using const_iterator = ListIterator<const T>;

    public:
        list() {
            empty_initialize_();
        }

        /// 接收初始化列表的方式构造
        list(std::initializer_list<T> init) {
            empty_initialize_();
            for (const auto &val: init) {
                push_back(val);
            }
        }

        /// 拷贝构造，仓库里面的代码没有拷贝构造 与=重载！！！！！
        list(const list &other)
                : list() {
            for (const auto &val: other) {
                push_back(val);
            }
        }

        /// 赋值运算符
        list &operator=(const list &other) {
            if (this != &other) {
                list temp(other);     /// 使用拷贝构造函数生成临时副本
                swap(temp);         /// 交换资源，实现强异常安全保证
            }
            return *this;
        }

        list &operator=(std::initializer_list<T> ilist) {
            list temp(ilist);         /// 利用已有的 initializer_list 构造函数
            swap(temp);             /// 交换资源，实现强异常安全保证
            return *this;
        }

        ~list() {
            clear();
            destroy_node_(node_);
        }

        /// 迭代器相关函数
        iterator begin() {
            return iterator(node_->next_);
        }

        iterator end() {
            return iterator(node_);
        }

        const_iterator begin() const {
            return const_iterator(node_->next_);
        }

        const_iterator end() const {
            return const_iterator(node_);
        }

        ///-------------insert()/erase()/push/pop
        /// 插入一个
        iterator insert(iterator pos, const T &value) {
#if 0
            std::cout << "insert called with pos.node_: " << pos.node_ << std::endl;
        if (pos.node_ != nullptr) {
            std::cout << "pos.node_->prev_: " << pos.node_->prev_ << std::endl;
            std::cout << "pos.node_->next_: " << pos.node_->next_ << std::endl;
        } else {
            std::cout << "pos.node_ is nullptr!" << std::endl;
        }
        assert(pos.node_ != nullptr);
        assert(pos.node_->prev_ != nullptr);
        std::cout << "pos.node_: " << pos.node_ << ", pos.node_->prev_: " << pos.node_->prev_ << std::endl;
#endif

            /// 双向链表中插入新节点的过程一致
            link_type new_node = create_node_(value);
            link_type cur = pos.node_;
            link_type prev = cur->prev_;
            /// 插入
            new_node->next_ = cur;
            new_node->prev_ = prev;
            prev->next_ = new_node;
            cur->prev_ = new_node;

            return iterator(new_node);
        }

        /// 插入n个value
        void insert(iterator pos, size_type n, const T &value) {
            for (size_type i = 0; i < n; ++i) {
                insert(pos, value);  /// 循环插入
            }
        }

        /// 以初始化列表的方式插入
        void insert(iterator pos, std::initializer_list<T> ilist) {
            insert(pos, ilist.begin(), ilist.end());  /// 调用下面的模板进行插入
        }

        /// insert模板
        template<typename InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last) {
            //// 也是使用循环插入
            for (auto it = first; it != last; ++it) {
                insert(pos, *it);       //// 将输入迭代器中的内容插入到list中
            }
        }

        ///-------------erase()
        iterator erase(iterator pos) {

            link_type prev_node = pos.node_->prev_;
            link_type next_node = pos.node_->next_;

#if 1
            /// debug
            std::cout << "\nerase called with pos.node_: " << pos.node_ << std::endl;
            if (pos.node_ != nullptr) {
                std::cout << "pos.node_->prev_: " << pos.node_->prev_ << std::endl;
                std::cout << "pos.node_->next_: " << pos.node_->next_ << std::endl;
            } else {
                std::cout << "pos.node_ is nullptr!" << std::endl;
            }
            assert(pos.node_ != nullptr);
            assert(pos.node_->prev_ != nullptr);
            assert(pos.node_->next_ != nullptr);
            std::cout << "pos.node_: " << pos.node_ << ", pos.node_->prev_: " << pos.node_->prev_
                      << ", pos.node_->next_: " << pos.node_->next_ << std::endl;
            /// debug
#endif

            //// 修改指向
            prev_node->next_ = next_node;
            next_node->prev_ = prev_node;
            destroy_node_(pos.node_);   //// 删除pos对应的node_
            return iterator(next_node);
        }

        /// 范围 erase
        iterator erase(iterator first, iterator last) {
            while (first != last) {
                //// 这里要十分注意迭代器失效
                first = erase(first);
            }
            return last;
        }

        ////-------------push_back()/push_front()
        void push_back(const T &value) {
            //// 链表尾部插入
            insert(end(), value);
        }

        void push_front(const T &value) {
            //// 链表尾部插入
            insert(begin(), value);
        }

        void pop_back() {
            //// 尾部删除，则用erase从尾部删除
            //// 这里需要注意end()尾迭代器是指向的尾元素的下一个位置，是一个空，在删除之前应该将end()向前移动一位
            erase(--end());
        }

        void pop_front() {
            //// 从前方删除只需要删除begin()指向的位置就行
            erase(begin());
        }

        ////-------------front()  back()  取出头部元素和尾部元素
        reference front() {
            return *begin();
        }

        reference back() {
            return *(--end());
        }

        /// 修改器函数  splic merge unique remove remove_if
        /// splice 将其他链表中的节点（或部分节点）插入到本链表的指定位置 pos 前面

        /// 整段移动--将 other 整个链表中的所有元素，插入到当前链表的 pos 位置之前
        /// 转移后 other 变为空链表，所有权转移
        void splice(iterator pos, list &other) {
            if (other.empty()) {
                return;
            }
            transfer(pos, other.begin(), other.end());

            /// 补充操作--->将 other 的哨兵节点指针指回自身，实现“空链表”状态
            /// 保证 other 为空链表，方便后续调用 empty() 正确返回 true
            other.node_->next_ = other.node_;
            other.node_->prev_ = other.node_;
        }

        /// 区间移动
        void splice(iterator pos, list &other, iterator first, iterator last) {
            if (first != last) {
                transfer(pos, first, last);
            }
        }

        /// 单个节点移动---将 other 中的 单个节点 it 移动到当前链表中 pos 位置之前
        void splice(iterator pos, list &other, iterator it) {
            iterator next = it;
            ++next;  /// ++next 是为了构造区间 [it, it+1)，也就是一个单独的节点
            /// pos == it：表示目标位置就是自身，不动；
            /// pos == it+1：表示把自己插到自己后面，也没必要动
            if (pos == it || pos == next) {
                return;
            }
            transfer(pos, it, next);    /// 进行单点移动
        }

        /// transfer()，将[first, end)范围内的数据移动到pos之前
        /// 主要作用是断开 [first, end)与前后元素的连接
        /// 不需要多余的拷贝、移动等，完成的时间复杂度是O（1）
        void transfer(iterator pos, iterator first, iterator end) {
            /// 如果插入的位置pos == end 说明已经在对应的位置了不需要移动
            if (pos != end) {
#if 0
                /// 将[first,end)的最后一个节点的下一个节点指向pos.node_
            end.node_->prev_->next_ = pos.node_;
            /// 断开first原本的连接，让其前一个节点连接到end节点
            first.node_->prev_->next_ = end.node_;
            /// pos位置的前一个节点应该连接first
            pos.node_->prev_->next_ = first.node_;
            /// 创建一个临时节点先保存pos的前一个节点的值
            link_type tmp = pos.node_->prev_;
            /// pos的前一个节点应该变成end()
            pos.node_->prev_ = end.node_->prev_;
            /// 在原始位置的end位置应该连接到原始的first节点的前一个节点
            end.node_->prev_ = first.node_->prev_;
            /// 现在的first节点应该连接到pos的前一个节点上
            first.node_->prev_ = tmp;
#endif
                /// 保存区间前后节点指针
                link_type first_prev = first.node_->prev_;
                link_type last_prev = end.node_->prev_;
                link_type pos_prev = pos.node_->prev_;

                /// 断开[first, end)这个区间
                first_prev->next_ = end.node_;
                end.node_->prev_ = first_prev;

                /// 把[first, end)插入到pos之前
                pos_prev->next_ = first.node_;
                first.node_->prev_ = pos_prev;

                last_prev->next_ = pos.node_;
                pos.node_->prev_ = last_prev;
            }
        }

        ////-------------merge
        /***
         *  merge()-->将另一个有序链表 other 合并到当前链表中，保持整体有序 稳定升序合并
         *  关键点：
         *  1、假设*this和other都是升序排列
         *  2、每次比较*first2 < *first1，把 first2 插到 first1 前面。
         *  3、如果当前链表到末尾，直接将 other 剩下的元素全部接到后面。
         *  ***/
        void merge(list &other) {
            iterator first1 = begin();
            iterator last1 = end();
            iterator first2 = other.begin();
            iterator last2 = other.end();

            while (first1 != last1 && first2 != last2) {
                if (*first2 < *first1) {
                    iterator next = first2;
                    ++next;
                    transfer(first1, first2, next);
                    first2 = next;
                } else {
                    ++first1;
                }
            }

            /// 如果一个链表还有空余，直接插入到前一个链表后面
            if (first2 != last2) {
                transfer(last1, first2, last2);
            }
        }

        /**
         * unique()
         *      去除连续重复元素（不排序，仅去重重复段）
         *  关键点：
         *      1、只去除相邻相等的元素
         *      2、遍历过程中跳过不同的值，遇到连续相同的就删掉
         * */
        void unique() {
            iterator first = begin();
            iterator last = end();
            if (first == last) {
                return;
            }
            iterator next = first;
            while (++next != last) {
                /// 如果当前相邻的节点元素相等的话就删
                if (*first == *next) {
                    erase(next);
                } else {
                    first = next;
                }
                next = first;
            }
        }

        /**
        * remove(const value_type& value)
        *      删除所有值等于 value 的节点。
        *  关键点：
        *      1、遍历时需要提前保存next，防止erase之后失效
        * */
        void remove(const value_type &value) {
            iterator first = begin();
            iterator last = end();
            while (first != last) {
                iterator next = first;
                ++next;
                if (*first == value) {
                    erase(first);
                }
                first = next;
            }
        }

        /**
       * remove_if(UnaryPredicate p)
       *      删除所有满足条件的元素的节点。
       *  关键点：
       *      1、传入一个函数、lambda 表达式或函数对象，返回 true 则删除。
       * */
        template<typename UnaryPredicate>
        void remove_if(UnaryPredicate p) {
            iterator first = begin();
            iterator last = end();
            while (first != last) {
                iterator next = first;
                ++next;
                if (p(*first)) {
                    erase(first);
                }
                first = next;
            }
        }

        ///-------------swap() 交换两个链表的内容
        void swap(list &other) {
            std::swap(node_, other.node_);
        }

        ////-------------resize()
        void resize(size_type count, const T &value) {
            //// 在重新分配大小的时候，要判断需要分配的数量与现有的数量大小的关系
            size_type cur = size();
            if (cur < count) {
                //// 现有的数量小于需要扩容的大小，直接在尾部插入
                insert(end(), count - cur, value);
            } else if (cur > count) {
                //// 现有的数量大于需要重置的数量大小，则需要删除末尾的部分元素
                auto it = begin();
                std::advance(it, count);
                erase(it, end()); // 删除 [count, end)
            }
        }

        void resize(size_type count) {
            resize(count, T());  /// 默认构造的 T
        }


        /// 重载==
        bool operator==(const list &other) const {
            auto first1 = begin();
            auto last1 = end();
            auto first2 = other.begin();
            auto last2 = other.end();

            while (first1 != last1 && first2 != last2) {
                if (*first1 != *first2) {
                    return false;
                }
                ++first1;
                ++first2;
            }

            return first1 == last1 && first2 == last2;
        }

        /**
         * std::list::sort() 的经典实现方式-->归并排序
         *  为什么list不用快排？
         *      快排对随机性要求很高，链表是顺序访问
         *  使用归并排序的原因：
         *      1、不需要额外的空间
         *      2、能在线性时间内合并两个有序链表
         *      3、稳定排序
         *
         *  关键参数：
         *      carry 是临时链表：从原链表取出一个元素；
         *      counter[i] 保存当前已经有的大小为 2^i 的“已排序子链表”；
         *      fill 表示目前用了几个 counter 槽位
         *
         *   这个过程相当于把每个节点看作一个“排序好的链表”，逐步做二进制式的归并
         *      相当于用 carry + counter 实现归并树结构；
         *      一旦 counter[i] 有值，就 merge 合并形成更大的链表（2, 4, 8...）
         * */
        void sort() {
            if (node_->next_ == node_ || node_->next_->next_ == node_) {
                // 链表为空或者只有一个元素
                return;
            }
            /// 初始化辅助结构
            list carry;
            list counter[64]; /// counter[i] 表示 2^i 个元素组成的子链表
            int fill = 0;

            /// 逐个取出节点，归并插入 counter[]
            //// 这个过程相当于把每个节点看作一个“排序好的链表”，逐步做二进制式的归并
            ///     相当于用 carry + counter 实现归并树结构；
            ///     一旦 counter[i] 有值，就 merge 合并形成更大的链表（2, 4, 8...）
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());           /// 取出一个元素到carry
                int i = 0;
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);       /// 合并已有 counter[i] 和 carry
                    carry.swap(counter[i++]);      /// carry 接替位置，准备继续合并更高位
                }
                carry.swap(counter[i]);            /// 最终 carry 放入空的 counter[i]
                if (i == fill) {
                    ++fill;
                }
            }

            ///  最终合并所有 counter 中的链表
            for (int i = 1; i < fill; ++i) {
                /// 从 counter[0] 开始，依次 merge 到 counter[fill - 1]
                counter[i].merge(counter[i - 1]);
            }
            /// 通过 swap 将其放入当前链表中。
            swap(counter[fill - 1]);        /// 最终结果放入当前 list 中
        }


        ////-------------size()
        size_type size() {
            size_type count = 0;
            for (auto it = begin(); it != end(); ++it) {
                ++count;
            }
            return count;
        }

        ////-------------empty()
        bool empty() {
            return node_->next_ == node_;        //// 双向链表判断为空，可以判断本节点的下一个节点是不是自己
        }

        //// 析构每一个节点
        void clear() {
            link_type cur = node_->next_;
            while (cur != node_) {
                link_type tmp = cur;
                cur = cur->next_;
                destroy_node_(tmp);
            }
            node_->next_ = node_;
            node_->prev_ = node_;
        }

    private:

        /// 在迭代器范围内初始化
        template<typename InputIterator>
        void range_initialize_(InputIterator first, InputIterator last) {
            /// 先做空初始化
            empty_initialize_();
            /// 然后进行插入
            insert(begin(), first, last);
        }

        /// 分配一个节点空间
        template<typename... Args>
        link_type get_node_(Args &&... args) {
            link_type p = allocator_.allocate(1);
            new(p) ListNode<T>(std::forward<Args>(args)...);  /// 直接初始化节点
            return p;
        }


        /// 创建一个节点
        template<typename... Args>
        link_type create_node_(Args &&... args) {
            link_type p = allocator_.allocate(1);
            /// 使用 placement new 调用 ListNode<T> 的构造函数，保证 prev_ 和 next_ 初始化
            new(p) ListNode<T>(std::forward<Args>(args)...);
            return p;
        }

        void destroy_node_(link_type p) {
//        std::cout << "[destroy_node_] at: " << p << std::endl;
            p->~ListNode<T>();  /// 显式调用整个节点的析构函数
            allocator_.deallocate(p, 1);
        }


        void put_node_(link_type p) {
            allocator_.deallocate(p, 1);
        }


        void empty_initialize_() {
            //// 初始化时获取一个节点，构造一个空的链表，在双向链表中为空时，前向指针和后向指针都会指向自己
            node_ = get_node_();
            node_->next_ = node_;
            node_->prev_ = node_;
        }

        link_type get_node_() {
            link_type p = allocator_.allocate(1);
            new(p) ListNode<T>();  /// 要求 ListNode<T> 有默认构造
            return p;
        }

    private:
        link_type node_;                    // 哨兵节点
        allocator_type allocator_;

    };

}