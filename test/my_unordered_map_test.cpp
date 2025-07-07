/**
 * @file      my_unordered_map_test.cpp
 * @brief     [unordered_map_test]
 * @author    Weijh
 * @date      2025/7/6
 * @version   1.0
 */

#include "my_unordered_map.h"
#include "my_string.h"

#include <cassert>
#include <iostream>


void test_basic_insert_and_find() {
    my::unordered_map<int, std::string> map;
    map.insert({1, "one"});
    map.insert({2, "two"});
    assert(map.size() == 2);
    assert(map.find(1)->second == "one");
    assert(map.find(2)->second == "two");
    std::cout << "test_basic_insert_and_find success.\n";
}

void test_operator_indexing() {
    my::unordered_map<std::string, int> map;
    map["a"] = 10;
    assert(map["a"] == 10);
    assert(map["b"] == 0);  // default-constructed
    map["b"] += 1;
    assert(map["b"] == 1);
    std::cout << "test_operator_indexing success.\n";
}

void test_erase_and_size() {
    my::unordered_map<int, std::string> map;
    map.insert({1, "a"});
    map.insert({2, "b"});
    assert(map.erase(1) == 1);
    assert(map.find(1) == map.end());
    assert(map.size() == 1);
    std::cout << "test_erase_and_size success.\n";
}

void test_at_and_exception() {
    my::unordered_map<int, std::string> map;
    map.insert({5, "five"});
    assert(map.at(5) == "five");

    bool caught = false;
    try {
        map.at(99);  // should throw
    } catch (const std::out_of_range &) {
        caught = true;
    }
    assert(caught);
    std::cout << "test_at_and_exception success.\n";
}

void test_swap_and_clear() {
    my::unordered_map<int, std::string> a, b;
    a.insert({1, "a"});
    b.insert({2, "b"});
    a.swap(b);
    assert(a.find(2) != a.end());
    assert(b.find(1) != b.end());

    a.clear();
    assert(a.empty());
    std::cout << "test_swap_and_clear success.\n";
}

int main() {
    test_basic_insert_and_find();
    test_operator_indexing();
    test_erase_and_size();
    test_at_and_exception();
    test_swap_and_clear();

    std::cout << "all test passed.\n";
    /**
        test_basic_insert_and_find success.
        test_operator_indexing success.
        test_erase_and_size success.
        test_at_and_exception success.
        test_swap_and_clear success.
        all test passed.

        进程已结束，退出代码为 0
     * */

    return 0;
}
