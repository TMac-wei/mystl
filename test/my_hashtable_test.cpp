/**
 * @file      my_hashtable_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#include "my_hashtable.h"

#include <cassert>
#include <string>
#include <iostream>


using std::cout;
using std::endl;

struct MyHash {
    size_t operator()(const int& x) const { return std::hash<int>()(x); }
};

struct MyEqual {
    bool operator()(const int& a, const int& b) const { return a == b; }
};

/// 测试唯一键插入和查找
void test_unique_insert_find()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type, MyHash, MyEqual>;
    Table ht(10);
    ht.insert(42);
    ht.insert(7);

    assert(ht.find(42) != ht.end());
    assert(ht.find(7) != ht.end());
    assert(ht.find(100) == ht.end());

    cout << "test_unique_insert_find passed.\n";
}

/// 测试重复键插入与 count
void test_multi_insert_count()
{
    using Table = my::hashtable<int, int, my::_Identify, std::false_type, MyHash, MyEqual>;
    Table ht(10);
    ht.insert(5);
    ht.insert(5);
    ht.insert(10);
    ht.insert(5);

    ht.debug_table();

    assert(ht.count(5) == 3);
    assert(ht.count(10) == 1);
    assert(ht.count(100) == 0);

    cout << "test_multi_insert_count passed.\n";
}

/// 拷贝构造函数/拷贝赋值测试
void test_copy_constructor_assignment()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type>;
    Table ht1(10);
    ht1.insert(1);
    ht1.insert(2);
    ht1.insert(3);

    Table ht2 = ht1;  // 拷贝构造
    assert(ht2.size() == 3);
    assert(ht2.find(2) != ht2.end());

    Table ht3(5);
    ht3 = ht1;        // 拷贝赋值
    assert(ht3.size() == 3);
    assert(ht3.find(3) != ht3.end());

    cout << "test_copy_constructor_assignment passed.\n";
}


void test_move_constructor_assignment()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type>;
    Table ht1(10);
    ht1.insert(11);
    ht1.insert(22);

    Table ht2 = std::move(ht1);
    assert(ht2.size() == 2);
    assert(ht2.find(11) != ht2.end());

    Table ht3(3);
    ht3 = std::move(ht2);
    assert(ht3.size() == 2);
    assert(ht3.find(22) != ht3.end());

    cout << "test_move_constructor_assignment passed.\n";
}

void test_iterator()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type>;
    Table ht(10);
    ht.insert(1);
    ht.insert(2);
    ht.insert(3);

    int sum = 0;
    for (auto it = ht.begin(); it != ht.end(); ++it) {
        sum += *it;
    }
    assert(sum == 6);

    cout << "test_iterator passed.\n";
}


void test_comparison()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type>;
    Table ht11(10);
    ht11.insert(5);
    ht11.insert(6);

    Table ht21 = ht11;
    assert(ht11 == ht21);

    ht21.insert(7);
    assert(ht11 != ht21);

    cout << "test_comparison passed.\n";
}

void test_clear_and_load()
{
    using Table = my::hashtable<int, int, my::_Identify, std::true_type>;
    Table ht(10);
    for (int i = 0; i < 5; ++i)
        ht.insert(i);

    assert(ht.size() == 5);
    float lf = ht.load_factor();
    assert(lf > 0.0f && lf <= 1.0f);

    ht.clear();
    assert(ht.size() == 0);
    assert(ht.empty());

    cout << "test_clear_and_load passed.\n";
}


int main()
{
    test_unique_insert_find();                  /// 过
    test_multi_insert_count();                  /// 过
    test_copy_constructor_assignment();         /// 过
    test_move_constructor_assignment();
    test_iterator();                              /// 过
    {
        test_comparison();
    }
    test_clear_and_load();
    /// 过
    cout << "All hashtable tests passed.\n";
    /**
     * test_unique_insert_find passed.
        === Hashtable Buckets Dump ===
        Bucket[0]: null
        Bucket[1]: null
        Bucket[2]: null
        Bucket[3]: null
        Bucket[4]: null
        Bucket[5]: 5 -> 5 -> 5 -> null
        Bucket[6]: null
        Bucket[7]: null
        Bucket[8]: null
        Bucket[9]: null
        Bucket[10]: 10 -> null
        ==============================
        test_multi_insert_count passed.
        test_copy_constructor_assignment passed.
        test_move_constructor_assignment passed.
        test_iterator passed.
        test_comparison passed.
        test_clear_and_load passed.
        All hashtable tests passed.
     * */
    return 0;
}




