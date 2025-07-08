/**
 * @file      my_my::unordered_set_test.cpp
 * @brief     [my::unordered_set_test]
 * @author    Weijh
 * @version   1.0
 */
#include "my_unordered_set.h"
#include <iostream>
#include <vector>
#include <cassert>

void test_unordered_set() {
    my::unordered_set<int> uset;

    // insert
    auto [it1, inserted1] = uset.insert(10);
    auto [it2, inserted2] = uset.insert(20);
    auto [it3, inserted3] = uset.insert(10); // duplicate
    auto [it4, inserted4] = uset.insert(10); // duplicate
    auto [it5, inserted5] = uset.insert(10); // duplicate
    auto [it6, inserted6] = uset.insert(30); // duplicate
    std::cout << "Inserted 10? " << inserted1 << std::endl;

    for (const auto& v : uset) {
        std::cout << "Value in uset: " << v << "\n";
    }


    assert(inserted1 == true);
    assert(inserted2 == true);
    assert(inserted3 == false);
    std::cout << "Inserted 10 again? " << inserted3 << std::endl;
    assert(uset.size() == 3);

    // find
    assert(uset.find(10) != uset.end());
    assert(uset.find(30) != uset.end());

    // erase by key
    size_t removed = uset.erase(10);
    std::cout << "Erased count: " << removed << std::endl;
    assert(removed == 1);
    assert(uset.count(10) == 0);

    // erase by iterator
    /// 每次插入节点都是采用头插法，那么在删除第一个节点，返回的是下一个节点的迭代器，这里断言应该是 it != end()
    uset.insert(100);
    auto it = uset.find(100);
    it = uset.erase(it);
    assert(it != uset.end());
    assert(uset.count(100) == 0);

    // range insert
    std::vector<int> vec = {1, 2, 3, 4};
    my::unordered_set<int> uset2(vec.begin(), vec.end());
    assert(uset2.size() == 4);

    // range erase
    uset2.erase(uset2.begin(), uset2.end());
    /// 这里删除之后没有找到下一个节点，导致没有进行删除，因为这四个节点都位于不同的桶上
    assert(uset2.empty());

    // copy & move
    uset.insert(77);
    my::unordered_set<int> copied(uset);
    assert(copied.count(77) == 1);

    my::unordered_set<int> moved(std::move(copied));
    assert(moved.count(77) == 1);
    assert(copied.size() == 0); // 若支持 move 后有效性，这一行可删

    std::cout << "All unordered_set tests passed.\n";
}

void test_unordered_set_unique_insert()
{
    my::unordered_set<int> uset;

    auto [it1, inserted1] = uset.insert(42);
    assert(inserted1 == true);
    std::cout << "Inserted 42: " << inserted1 << std::endl;

    auto [it2, inserted2] = uset.insert(42);
    assert(inserted2 == false); // 不应重复插入
    std::cout << "Inserted 42 again: " << inserted2 << std::endl;

    assert(uset.size() == 1);

    size_t erased = uset.erase(42);
    assert(erased == 1);

    auto it3 = uset.find(42);
    assert(it3 == uset.end());

    std::cout << "All test_unordered_set_unique_insert tests passed." << std::endl;
}

int main() {
    test_unordered_set_unique_insert();
    test_unordered_set();


    return 0;
}
