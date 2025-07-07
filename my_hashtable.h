/**
 * @file      my_hashtable.h
 * @brief     [hashtable实现]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#pragma once

#include "my_vector.h"
#include "my_utility.h"
#include "my_type_traits.h"
#include "my_pair.h"

#include <cmath>
#include <functional>
#include <memory>
#include <cassert>

/**
 * hashtable
 * 哈希函数：将key映射到哈希表的索引，理想的哈希函数应该是均匀的，
 *          即每个key都有相同的概率映射到哈希表的每个索引桶：哈希表的每个索引对应一个桶，桶是一个链表，用于解决哈希冲突
 * 哈希冲突：不同的key映射到哈希表的同一个索引，解决哈希冲突的方法有链地址法、开放地址法、再哈希法，标准库中使用链地址法
 * 负载因子：哈希表中元素个数和桶的比值，负载因子越大，哈希冲突越多，查找效率越低，标准库中负载因子为1
 * 重新哈希：当负载因子大于1时，需要重新哈希，即重新分配桶的个数，将所有元素重新插入到新的桶中
 *
 */

namespace my {
    /// 哈希表采用链地址法，所以需要创建节点
    template<typename Value>
    struct HashTableNode {
        Value value;            /// 存储的数据
        HashTableNode *next;    /// 指向下一个节点指针

        HashTableNode(const Value &val)
                : value(val), next(nullptr) {}

        HashTableNode(Value &&val)
                : value(my::move(val)), next(nullptr) {}
    };

    /// hashtable前置声明
    template<typename Key,
            typename Value,
            typename ExtractKey,
            typename UniqueKeys,
            typename Hash,
            typename EqualKey,
            typename Alloc>
    class hashtable;

    /// hashtable_iterator
    template<typename Key,                 /// 哈希表中元素的键类型（如 int、std::string）
            typename Value,               /// 哈希表存储的值类型（可能是 pair<const Key, T>）
            typename ExtractKey,          /// 从 Value 中提取 Key 的函数或仿函数类型
            typename UniqueKeys,          /// 用于区分是否是唯一键（如 unordered_map）还是多值键（如 unordered_multimap）
            typename Hash,                /// 哈希函数类型
            typename EqualKey,            /// 键相等比较器
            typename Alloc,               /// 分配器类型
            bool IsConst = false>        /// 迭代器是否是const类型
    class hashtable_iterator {
    public:
        /// 定义类型
        using iterator_type = hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, IsConst>;
        using iterator_category = std::forward_iterator_tag;    /// 前向迭代器
        using hashtable_type = hashtable<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc>;

        using value_type = Value;
        using difference_type = std::ptrdiff_t;

        using pointer = my::conditional_t<IsConst, const Value *, Value *>;;
        using reference = my::conditional_t<IsConst, const Value &, Value &>;
        using hashtable_pointer = my::conditional_t<IsConst, const hashtable_type *, hashtable_type *>;

        using node = HashTableNode<Value>;
        using node_pointer = my::conditional_t<IsConst, const node*, node*>;
        using node_reference = node &;
        using const_iterator = hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, true>;

        /// 设置hashtable友元
        friend class hashtable<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc>;

        template<typename K, typename V, typename E, typename U, typename H, typename EQ, typename A, bool C>
        friend
        class hashtable_iterator;

    public:

        /// 构造函数
        /// 默认构造函数，构造空迭代器（用于 end() 迭代器）
        hashtable_iterator()
                : current_(nullptr), table_(nullptr) {}

        hashtable_iterator(node_pointer current, hashtable_pointer table)
                : current_(current), table_(table) {}

        /// 构造函数
        hashtable_iterator(const hashtable_iterator &other)
                : current_(other.current_), table_(other.table_) {}

        /// 允许从 非const 转换为 const_iterator
        template<bool B = IsConst, typename = my::enable_if_t<B>>
        hashtable_iterator(
                const hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, false> &other)
                : current_(other.current_), table_(other.table_) {}


        /// 允许从 const_iterator 转换为非 const iterator
        template <bool B = IsConst, std::enable_if_t<!B, int> = 0>
        hashtable_iterator(const hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, true>& other)
                : current_(const_cast<node*>(other.current_)), table_(const_cast<hashtable_type*>(other.table_)) {}


        /// 解引用
        reference operator*() {
            /// 检查空指针
            assert(current_ && "Dereferencing end() iterator");
            return current_->value;
        }

        pointer operator->() {
            assert(current_ && "Accessing end() iterator");
            return &current_->value;
        }

        /// 前置++
        iterator_type &operator++() {
            if (current_) {
                /// 如果cur_的下一个节点存在就移动到下一个节点
                if (current_->next) {
                    current_ = current_->next;
                } else {
                    /// 当前桶没有下一个节点，则需要找到下一个current_的位置
                    /// 获取当前 value 的哈希桶索引（通过 bucket_index_ 函数）
                    size_t index = table_->bucket_index_(current_->value);
                    ++index;
                    /// 向后查找下一个非空桶，并更新为该桶的第一个节点
                    while (index < table_->buckets_.size() && table_->buckets_[index] == nullptr) {
                        ++index;
                    }
                    if (index < table_->buckets_.size()) {
                        current_ = table_->buckets_[index];
                    } else {
                        /// 将 current_ 置为 nullptr，表示 end()
                        current_ = nullptr;
                    }
                    /// 可以优化为三目运算
//                        current_ = (index < table_->buckets_.size()) ? table_->buckets_[index] : nullptr;
                }
            }
            return *this;
        }

        iterator_type operator++(int) {
            hashtable_iterator temp = *this;
            ++(*this);
            return temp;
        }

        /// == !=  两个迭代器仅比较当前节点是否相同，表指针 table_ 不参与判断
        bool operator==(const iterator_type &other) const {
            return current_ == other.current_;
        }

        bool operator!=(const iterator_type &other) const {
            return !(*this == other);
        }

        /// 成员变量  用于追踪当前位置和原表
        node_pointer current_;        /// 当前所在节点的指针
        hashtable_pointer table_;


    };

    /**
 * 接下来detail作用域内，给出一些辅助函数的内容
 * */
    namespace detail {
        // The sentinel value is kept only for abi backward compatibility.
        /// 预定义的素数数组，按照升序排列
        extern const unsigned long prime_list__[] =  // 256 + 1 or 256 + 48 + 1
                {2ul, 3ul, 5ul, 7ul, 11ul, 13ul, 17ul, 19ul,
                 23ul, 29ul, 31ul, 37ul, 41ul, 43ul, 47ul, 53ul,
                 59ul, 61ul, 67ul, 71ul, 73ul, 79ul, 83ul, 89ul,
                 97ul, 103ul, 109ul, 113ul, 127ul, 137ul, 139ul, 149ul,
                 157ul, 167ul, 179ul, 193ul, 199ul, 211ul, 227ul, 241ul,
                 257ul, 277ul, 293ul, 313ul, 337ul, 359ul, 383ul, 409ul,
                 439ul, 467ul, 503ul, 541ul, 577ul, 619ul, 661ul, 709ul,
                 761ul, 823ul, 887ul, 953ul, 1031ul, 1109ul, 1193ul, 1289ul,
                 1381ul, 1493ul, 1613ul, 1741ul, 1879ul, 2029ul, 2179ul, 2357ul,
                 2549ul, 2753ul, 2971ul, 3209ul, 3469ul, 3739ul, 4027ul, 4349ul,
                 4703ul, 5087ul, 5503ul, 5953ul, 6427ul, 6949ul, 7517ul, 8123ul,
                 8783ul, 9497ul, 10273ul, 11113ul, 12011ul, 12983ul, 14033ul, 15173ul,
                 16411ul, 17749ul, 19183ul, 20753ul, 22447ul, 24281ul, 26267ul, 28411ul,
                 30727ul, 33223ul, 35933ul, 38873ul, 42043ul, 45481ul, 49201ul, 53201ul,
                 57557ul, 62233ul, 67307ul, 72817ul, 78779ul, 85229ul, 92203ul, 99733ul,
                 107897ul, 116731ul, 126271ul, 136607ul, 147793ul, 159871ul, 172933ul, 187091ul,
                 202409ul, 218971ul, 236897ul, 256279ul, 277261ul, 299951ul, 324503ul, 351061ul,
                 379787ul, 410857ul, 444487ul, 480881ul, 520241ul, 562841ul, 608903ul, 658753ul,
                 712697ul, 771049ul, 834181ul, 902483ul, 976369ul, 1056323ul, 1142821ul, 1236397ul,
                 1337629ul, 1447153ul, 1565659ul, 1693859ul, 1832561ul, 1982627ul, 2144977ul, 2320627ul,
                 2510653ul, 2716249ul, 2938679ul, 3179303ul, 3439651ul, 3721303ul, 4026031ul, 4355707ul,
                 4712381ul, 5098259ul, 5515729ul, 5967347ul, 6456007ul, 6984629ul, 7556579ul, 8175383ul,
                 8844859ul, 9569143ul, 10352717ul, 11200489ul, 12117689ul, 13109983ul, 14183539ul, 15345007ul,
                 16601593ul, 17961079ul, 19431899ul, 21023161ul, 22744717ul, 24607243ul, 26622317ul, 28802401ul,
                 31160981ul, 33712729ul, 36473443ul, 39460231ul, 42691603ul, 46187573ul, 49969847ul, 54061849ul,
                 58488943ul, 63278561ul, 68460391ul, 74066549ul, 80131819ul, 86693767ul, 93793069ul, 101473717ul,
                 109783337ul, 118773397ul, 128499677ul, 139022417ul, 150406843ul, 162723577ul, 176048909ul, 190465427ul,
                 206062531ul, 222936881ul, 241193053ul, 260944219ul, 282312799ul, 305431229ul, 330442829ul, 357502601ul,
                 386778277ul, 418451333ul, 452718089ul, 489790921ul, 529899637ul, 573292817ul, 620239453ul, 671030513ul,
                 725980837ul, 785430967ul, 849749479ul, 919334987ul, 994618837ul, 1076067617ul, 1164186217ul,
                 1259520799ul,
                 1362662261ul, 1474249943ul, 1594975441ul, 1725587117ul, 1866894511ul, 2019773507ul, 2185171673ul,
                 2364114217ul,
                 2557710269ul, 2767159799ul, 2993761039ul, 3238918481ul, 3504151727ul, 3791104843ul, 4101556399ul,
                 4294967291ul};
        const int32_t prime_list_size__ = 256;

        /// 选择质数和rehash的策略,使用素数作为桶数量，并基于 最大负载因子（load factor） 控制是否扩容
        struct prime_rehash_policy {
            prime_rehash_policy(float max_load_factor = 1.0f)
                    : max_load_factor_(max_load_factor) {}

            /// 返回当前的最大负载因子
            float max_load_factor() const {
                return max_load_factor_;
            }

            /// 根据输入的bkt数量找到离他最近的素数
            size_t next_bkt(size_t bkt) const {
                /// 找到第一个≥bkt的素数
                const unsigned long *p__ = std::lower_bound(prime_list__,
                                                            prime_list__ + prime_list_size__ - 1, bkt);

                if (p__ == prime_list__ + prime_list_size__ - 1 && bkt > *p__) {
                    throw std::overflow_error("Exceeded max supported bucket size in hashtable.");
                }

                /// 更新下一次扩容的阈值：桶数 × 负载因子
                next_resize_ = static_cast<size_t>(ceilf(*p__ * max_load_factor_));
                return *p__;        /// 返回找到的那个素数
            }

            /// 给定元素个数，计算最小所需桶数（即使负载刚好满），并返回下一个合适的素数桶数
            size_t bkt_for_elements(size_t num_elements) const {
                const float min_nkts = static_cast<float>(num_elements) / max_load_factor_;
                return next_bkt(static_cast<size_t>(ceilf(min_nkts)));
            }

            /// rehash: 插入 n_ins 个元素之后，是否会导致负载因子超过上限，从而需要重新扩容（rehash）
            /// 如果需要扩容： 函数返回    std::make_pair(true, new_bucket_count);new_bucket_count 是新的桶数量
            ///     n_bkt: 当前桶数量
            ///     n_elt: 当前元素数量
            ///     n_ins: 即将插入的元素数量
            std::pair<bool, size_t> need_rehash(size_t n_bkt, size_t e_elt, size_t n_ins) const {
                /// 一定要考虑扩容
                if (e_elt + n_ins > next_resize_) {
                    /// 考虑扩容
                    float min_bkts = static_cast<float>(e_elt + n_ins) / max_load_factor_;
                    if (min_bkts > n_bkt) {
                        /// 增加桶的数量
                        min_bkts = std::max(min_bkts, growth_factor_ * n_bkt);
                        return std::make_pair(true, next_bkt(ceilf(min_bkts)));
                    } else {
                        next_resize_ = static_cast<std::size_t>(ceilf(n_bkt * max_load_factor_));
                        return std::make_pair(false, 0);
                    }
                } else {
                    return std::make_pair(false, 0);
                }
            }

            enum {
                num_primes = prime_list_size__
            };

            /// 成员变量
            float max_load_factor_;             /// 负载因子
            float growth_factor_{2.0f};         ///  每次 rehash 后，桶的最小增长倍数（默认 2 倍扩容）
            mutable size_t next_resize_{0};     /// 下次需要 resize 时的元素数量阈值。被 need_rehash 修改，所以是 mutable
        };
    }


    /// 接下来是关于hashtable
/**
 * 模板参数介绍
 *      typename UniqueKeys = std::true_type
 *          决定是否支持键唯一，std::true_type → 唯一（如 set/map），std::false_type → 可重复（如 multiset/multimap）
 *       typename Hash       = std::hash<Key>
 *          哈希函数，默认使用 std::hash<Key>
 *        typename EqualKey   = std::equal_to<Key>,
 *          键相等比较函数，默认使用 std::equal_to<Key>
 * */
    template<typename Key,
            typename Value,
            typename ExtractKey,
            typename UniqueKeys = std::true_type,
            typename Hash       = std::hash<Key>,
            typename EqualKey   = std::equal_to<Key>,
            typename Alloc      = std::allocator<Value>>
    class hashtable {
        /// 声明迭代器为友元，让迭代器可以访问私有成员
        friend class hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, false>;

        friend class hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, true>;

    public:
        using key_type = Key;
        using value_type = Value;
        using node_type = HashTableNode<Value>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using hasher = Hash;
        using key_equal = EqualKey;
        using allocator_type = Alloc;
        using node_allocator_type = typename std::allocator_traits<Alloc>::template rebind_alloc<node_type>;
        using reference = Value &;
        using const_reference = const Value &;
        using pointer = typename std::allocator_traits<Alloc>::pointer;
        using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
        using iterator = hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, false>;
        using const_iterator = hashtable_iterator<Key, Value, ExtractKey, UniqueKeys, Hash, EqualKey, Alloc, true>;
        using rehash_policy = detail::prime_rehash_policy;          /// detail这个作用域中的哈希策略
        using unique_keys = UniqueKeys;
    public:

        /// 构造函数遵循 rules of five
        explicit hashtable(size_type n)
                : hash_(hasher()), equals_(key_equal()), get_key_(ExtractKey()), num_elements_(0),
                  value_alloc_(allocator_type()) {
            node_alloc_ = node_allocator_type();
            /// 初始化buckets_大小
            buckets_.resize(rehash_policy_.next_bkt(n));
        }

        hashtable(const hashtable &other)
                : hash_(other.hash_), equals_(other.equals_), get_key_(other.get_key_),
                  buckets_(other.buckets_.size(), nullptr), num_elements_(0),
                  node_alloc_(std::allocator_traits<node_allocator_type>::
                              select_on_container_copy_construction(other.node_alloc_)),
                  value_alloc_(std::allocator_traits<allocator_type>::
                               select_on_container_copy_construction(other.value_alloc_)),
                  rehash_policy_(other.rehash_policy_) {
            /// 逐个复制所有节点
            for (size_t i = 0; i < other.buckets_.size(); ++i) {
                node_type *cur = other.buckets_[i];
                node_type **tail = &buckets_[i];
                while (cur) {
                    node_type *new_node = create_node_(cur->value);
                    *tail = new_node;
                    tail = &new_node->next;
                    cur = cur->next;
                    ++num_elements_;
                }
            }
        }

        hashtable &operator=(const hashtable &other) {
            if (this != &other) {
                hashtable temp(other);
                swap(temp);
            }
            return *this;
        }

        hashtable(hashtable &&other) noexcept
                : hash_(my::move(other.hash_)), equals_(my::move(other.equals_)), get_key_(my::move(other.get_key_)),
                  buckets_(my::move(other.buckets_)), num_elements_(other.num_elements_),
                  node_alloc_(my::move(other.node_alloc_)), value_alloc_(my::move(other.value_alloc_)),
                  rehash_policy_(my::move(other.rehash_policy_)) {
            other.num_elements_ = 0;
        }

        hashtable &operator=(hashtable &&other) noexcept {
            if (this != &other) {
                clear();

                hash_ = my::move(other.hash_);
                equals_ = my::move(other.equals_);
                get_key_ = my::move(other.get_key_);
                buckets_ = my::move(other.buckets_);
                num_elements_ = other.num_elements_;
                node_alloc_ = my::move(other.node_alloc_);
                value_alloc_ = my::move(other.value_alloc_);
                rehash_policy_ = my::move(other.rehash_policy_);

                other.num_elements_ = 0;
            }
            return *this;
        }

        ~hashtable() {
            clear();
        }

        /// 迭代器相关操作
        iterator begin() {
            /// 找到第一个非空桶，返回第一个节点
            for (size_t i = 0; i < buckets_.size(); i++) {
                if (buckets_[i]) {
                    return iterator(buckets_[i], this);
                }
            }
            return end();
        }

        const_iterator begin() const {
            /// 找到第一个非空桶，返回第一个节点
            for (size_t i = 0; i < buckets_.size(); i++) {
                if (buckets_[i]) {
                    return const_iterator(buckets_[i], this);
                }
            }
            return end();
        }

        const_iterator end() const {
            return const_iterator(nullptr, this);
        }

        iterator end() {
            /// 最后节点的下一个节点
            return iterator(nullptr, this);
        }

        const_iterator cbegin() const {
            for (size_t i = 0; i < buckets_.size(); ++i) {
                if (buckets_[i]) {
                    return const_iterator(buckets_[i], this);
                }
            }
            return const_iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }

        std::pair<iterator, bool> insert_dispatch(const value_type &value, std::true_type) {
            if (buckets_.empty()) {
                rehash_(rehash_policy_.next_bkt(1));
            }

            std::pair<bool, size_t> do_rehash = rehash_policy_.need_rehash(buckets_.size(), num_elements_, 1);
            if (do_rehash.first) {
                rehash_(do_rehash.second);
            }

            size_type idx = bucket_index_(value);
            node_type *cur = buckets_[idx];
            while (cur) {
                if (equals_(get_key_(cur->value), get_key_(value))) {
                    return {iterator(cur, this), false};
                }
                cur = cur->next;
            }

            node_type *node = create_node_(value);
            node->next = buckets_[idx];
            buckets_[idx] = node;
            ++num_elements_;
            return {iterator(node, this), true};
        }

        /// 可重复键版本（multiset/multimap）
        iterator insert_dispatch(const value_type &value, std::false_type) {
            if (buckets_.empty()) {
                rehash_(rehash_policy_.next_bkt(1));
            }

            std::pair<bool, size_t> do_rehash = rehash_policy_.need_rehash(buckets_.size(), num_elements_, 1);
            if (do_rehash.first) {
                rehash_(do_rehash.second);
            }

            size_type idx = bucket_index_(value);
            node_type *node = create_node_(value);
            node->next = buckets_[idx];
            buckets_[idx] = node;
            ++num_elements_;
            return iterator(node, this);
        }

        /// 插入
        auto insert(const value_type &value) {
            /// 根据unique_keys匹配模板判断是否可以重复插入，然后去调用对应版本的插入
            return insert_dispatch(value, unique_keys{});
        }

        /// 查询函数
        /// 按key查找
        iterator find(const key_type &key) {
            if (buckets_.empty()) {
                rehash_(rehash_policy_.next_bkt(1));
            }

            size_type idx = hash_(key) % buckets_.size();
            node_type *cur = buckets_[idx];
            while (cur) {
                if (equals_(get_key_(cur->value), key)) {
                    return iterator(cur, this);
                }
                cur = cur->next;
            }
            return end();       /// 没找到就返回尾迭代器
        }

        /// 迭代器删除 erase
        iterator erase(iterator it) {
            if (buckets_.empty()) {
                rehash_(rehash_policy_.next_bkt(1));
            }

            assert(it.table_ == this);

            /// 按照迭代器删除
            /// 先找到这个迭代器对应的桶
            node_type *cur = it.current_;
            size_type idx = bucket_index_(cur->value);
            node_type *first = buckets_[idx];

            /// 先保存下一个节点指针
            node_type *next = find_next_node(cur);

            /// 判断当前的桶是否和cur一致
            if (first == cur) {
                buckets_[idx] = cur->next;
                /// 释放这个节点对象
            } else {
                node_type *prev = first;    /// 如果暂时不相等的话，直接找到被删除迭代器的前一个迭代器
                while (prev->next != cur) {
                    prev = prev->next;
                }
                prev->next = cur->next;
            }

            destroy_node_(cur);
            --num_elements_;
            return next ? iterator(next, this) : end();         /// 如果prev的下一个节点还存在的话，就返回这个迭代器，否则返回end
        }

        /// 按照键删除,并且返回删除了多少个
        size_type erase(key_type key) {
            if (buckets_.empty()) {
                rehash_(rehash_policy_.next_bkt(1));
            }

            /// 按照键删除，直接用两个指针分别指向当前节点和当前节点的前一个节点，找到之后，进行删除
            size_type idx = hash_(key) % buckets_.size();
            node_type *cur = buckets_[idx];
            node_type *prev = nullptr;
            size_type cnt = 0;
            while (cur) {
                if (equals_(get_key_(cur->value), key)) {
                    /// cur找到了当前需要被删除的键
                    if (prev) {
                        /// 考虑是不是头节点，如果是头节点prev= nullptr
                        prev->next = cur->next;
                    } else {
                        buckets_[idx] = cur->next;      /// 每个桶地址就是其中的第一个节点的地址
                    }
                    node_type *next = cur->next;
                    destroy_node_(cur);
                    cur = next;
                    ++cnt;
                    --num_elements_;
                } else {
                    /// 没有找到相等的key
                    prev = cur;
                    cur = cur->next;
                }
            }
            return cnt;
        }

        /// 支持const_iterator重载
        iterator erase(const_iterator it) {
            return erase(iterator(it.current_, const_cast<hashtable *>(it.table_)));
        }



        /// 获取大小
        size_type size() const {
            return num_elements_;
        }

        /// empty
        bool empty() const {
            return num_elements_ == 0;
        }

        /// count计算目标键相等的有多少个
        size_type count(const key_type &key) const {
            size_type idx = hash_(key) % buckets_.size();
            node_type *cur = buckets_[idx];
            size_type cnt = 0;
            while (cur) {
                if (equals_(get_key_(cur->value), key)) {
                    ++cnt;
                }
                cur = cur->next;
            }
            return cnt;
        }

        /// load_factor
        float load_factor() const {
            return static_cast<float>(num_elements_) / buckets_.size();
        }

        /// 重载==与！=
        bool operator==(const hashtable &other) const {
            if (this == &other) {
                return true;
            }

            if (size() != other.size()) {
                return false;
            }

            /// 需要比较相同的桶上有相同数量的相同节点
            auto f1 = begin();
            auto l1 = end();
            auto f2 = other.begin();
            auto l2 = other.end();

            for (; f1 != l1 && f2 != l2; ++f1, ++f2) {
                if (*f1 != *f2) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const hashtable &other) const {
            return !(*this == other);
        }

        /// swap 将两者的所有资源交换
        void swap(hashtable &other) {
            my::swap(hash_, other.hash_);
            my::swap(equals_, other.equals_);
            my::swap(get_key_, other.get_key_);
            my::swap(buckets_, other.buckets_);
            my::swap(num_elements_, other.num_elements_);
            my::swap(node_alloc_, other.node_alloc_);
            my::swap(value_alloc_, other.value_alloc_);
            my::swap(rehash_policy_, other.rehash_policy_);
        }

        /// clear()
        void clear() {
            /// 将所有的节点对象析构
            for (size_t i = 0; i < buckets_.size(); i++) {
                /// 取出当前节点
                node_type *cur = buckets_[i];
                while (cur) {
                    /// 提前保存下一个节点
                    node_type *next = cur->next;
                    destroy_node_(cur);
                    cur = next;
                }
                buckets_[i] = nullptr;
            }
            /// 清空节点元素数量
            num_elements_ = 0;
        }

        /// 兼容唯一键和重复键的equal_range()实现
        my::pair<iterator, iterator> equal_range(const key_type &key) {
            if constexpr (UniqueKeys::value) {
                /// 唯一键模式（如 unordered_map），最多只有一个
                iterator it = find(key);
                if (it == end()) {
                    return {end(), end()};
                }
                iterator next = it;
                ++next;
                return {it, next};
            } else {
                /// 多键模式（如 unordered_multimap）
                size_type index = hash_index(key);
                node_type *node = buckets_[index];

                /// 找到第一个 key 匹配的位置
                while (node && !equals_(get_key_(node->value), key)) {
                    node = node->next;
                }

                node_type *first = node;

                /// 向后找，直到 key 不匹配为止
                while (node && equals_(get_key_(node->value), key)) {
                    node = node->next;
                }

                return {
                        iterator(first, this),
                        iterator(node, this)
                };
            }
        }

        /// const 版本的 equal_range()
        std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
            if constexpr (UniqueKeys::value) {
                /// 唯一键模式（如 unordered_map），最多只有一个
                const_iterator it = find(key);
                if (it == cend()) {
                    return {cend(), cend()};
                }
                const_iterator next = it;
                ++next;
                return {it, next};
            } else {
                /// 多键模式（如 unordered_multimap）
                size_type index = hash_index(key);
                const node_type *node = buckets_[index];  // 注意这里是 const node_type*

                /// 找到第一个 key 匹配的位置
                while (node && !equals_(get_key_(node->value), key)) {
                    node = node->next;
                }

                const node_type *first = node;

                /// 向后找，直到 key 不匹配为止
                while (node && equals_(get_key_(node->value), key)) {
                    node = node->next;
                }
                const node_type *last = node;

                return { const_iterator(first, this), const_iterator(last, this) };
            }
        }


        /// 获取hash索引
        size_type hash_index(const key_type &key) const {
            return hash_(key) % buckets_.size();
        }


        /// 验证是否正确插入多节点 添加下方调试函数
        void debug_table() const {
            std::cout << "=== Hashtable Buckets Dump ===\n";
            for (size_t i = 0; i < buckets_.size(); ++i) {
                std::cout << "Bucket[" << i << "]: ";
                node_type *cur = buckets_[i];
                while (cur) {
                    std::cout << cur->value << " -> ";
                    cur = cur->next;
                }
                std::cout << "null\n";
            }
            std::cout << "==============================\n";
        }

    private:
        /// 私有接口
        /// 找到当前节点cur的全局下一个节点
        node_type *find_next_node(node_type *cur) {
            size_type idx = bucket_index_(cur->value);
            /// 桶内还有下一个节点就直接返回
            if (cur->next) {
                return cur->next;
            }

            /// 否则往后找下一个非空桶
            for (size_type i = idx + 1; i < buckets_.size(); ++i) {
                if (buckets_[i]) {
                    return buckets_[i];
                }
            }
            /// 如果已经没有了则返回nullptr
            return nullptr;
        }

        /// 获取桶下标
        size_type bucket_index_(const value_type &value, size_t n) const {
            return hash_(get_key_(value)) % n;
        }

        size_type bucket_index_(const value_type &value) const {
            return bucket_index_(value, buckets_.size());
        }

        /// 创建节点和销毁节点
        node_type *create_node_(const value_type &value) {
            /// 先分配空间
            node_type *n = node_alloc_.allocate(1);
            try {
                /// 构造节点中的 value 成员
                std::allocator_traits<allocator_type>::construct(value_alloc_, std::addressof(n->value), value);
                /// 初始化 next 指针
                n->next = nullptr;      /// 如果不初始化的话，很容易造成野指针，导致未定义的随机值，后续崩溃
            } catch (...) {
                /// 如果构造失败，需要释放空间
                node_alloc_.deallocate(n, 1);
                throw;
            }
            return n;
        }

        void destroy_node_(node_type *node) {
            /// 先析构对象
            std::allocator_traits<allocator_type>::destroy(value_alloc_, std::addressof(node->value));
            /// 后释放内存
            node_alloc_.deallocate(node, 1);
        }

        /// rehash策略
        void rehash_(size_type n) {
            std::vector<node_type *> new_buckets(n);
            for (size_t i = 0; i < buckets_.size(); i++) {
                node_type *cur = buckets_[i];
                while (cur) {
                    /// 获取hash值
                    size_type idx = bucket_index_(cur->value, n);
                    /// 保存下一个节点
                    node_type *next = cur->next;
                    /// 指向新的桶数组
                    cur->next = new_buckets[idx];
                    new_buckets[idx] = cur;
                    cur = next;
                }
            }
            /// 交换新旧桶
            buckets_.swap(new_buckets);
        }

    private:
        /// 以下是核心成员变量
        hasher hash_;                            /// 哈希函数对象
        key_equal equals_;                      /// 键等价比较器
        ExtractKey get_key_;                    /// 提取 key 的函数对象
        std::vector<node_type *> buckets_;       /// 哈希函数桶，每个桶是一个链表头指针
        size_type num_elements_;                /// 当前元素的个数
        node_allocator_type node_alloc_;        /// 节点分配器
        allocator_type value_alloc_;            /// 值分配器
        rehash_policy rehash_policy_;           /// 重哈希策略
    };

/// 恒等函数对象---将输入直接原样返回
/// 在哈希表等容器中，当 key/value 是同一个类型时，key extractor 可用 _Identity
/// 实现如 std::unordered_set 时不需要从 value_type 中提取 key，直接返回自身即可
    struct _Identify {
        template<typename _Tp>
        _Tp &&operator()(_Tp &&__x) const {
            return my::forward<_Tp>(__x);
        }
    };

/// 选择pair的第一个元素
/// 提取 std::pair（或 std::tuple）中的第一个元素，并保留其值类别。
/// 常用于容器中提取 key
/// std::map<Key, T> 或 std::unordered_map<Key, T> 的元素类型是 std::pair<const Key, T>
/// 为了从 value_type 中提取出 key（pair 的 first），你就需要 _Select1st 函数对象
    struct _Select1st {
        template<typename _Tp>
        auto operator()(_Tp &&__x) const -> decltype(std::get<0>(my::forward<_Tp>(__x))) {
            return std::get<0>(my::forward<_Tp>(__x));
        };
    };
}