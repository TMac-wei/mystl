/**
 * @file      my_unordered_multimap_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "my_unordered_multiset.h"
#include <iostream>
#include <cassert>

int main() {
    my::unordered_multiset<int> ms;

    // 插入元素
    auto it1 = ms.insert(5);
    auto it2 = ms.insert(5);
    auto it3 = ms.insert(10);
    assert(ms.size() == 3);
    assert(ms.count(5) == 2);
    assert(ms.count(10) == 1);
    assert(ms.count(100) == 0);

    // 查找元素
    auto f = ms.find(5);
    assert(f != ms.end());
    assert(*f == 5);

    // 删除某个 key 的所有元素
    size_t erased = ms.erase(5);
    assert(erased == 2);
    assert(ms.count(5) == 0);

    // 单个迭代器删除
    ms.insert(20);
    ms.insert(30);
    auto it = ms.find(20);
    ms.erase(it);
    assert(ms.count(20) == 0);

    // 清空
    ms.clear();
    assert(ms.empty());

    // 拷贝与比较
    ms.insert(42);
    my::unordered_multiset<int> ms2 = ms;
    assert(ms2 == ms);

    // 交换
    my::unordered_multiset<int> ms3;
    ms3.insert(100);
    ms3.swap(ms2);
    assert(ms3.count(42) == 1);
    assert(ms2.count(100) == 1);

    std::cout << "All tests passed.\n";
    return 0;
}
