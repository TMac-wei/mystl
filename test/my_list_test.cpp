/**
 * @file      my_my::list_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

//
// Author:      Weijh 
// Created on:  2025/6/25.
//
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../my_list.h"

#if 1
void test_basic_insert_pop() {
    my::list<int> lst;
    lst.push_back(1);
    lst.push_front(0);
    lst.push_back(2);
    assert(lst.front() == 0);
    assert(lst.back() == 2);
    assert(lst.size() == 3);

    lst.pop_front();
    assert(lst.front() == 1);
    lst.pop_back();
    assert(lst.back() == 1);
    assert(lst.size() == 1);
    std::cout << "test_basic_insert_pop() success!\n";
}

void test_iterator_access() {
    my::list<int> lst;
    for (int i = 1; i <= 5; ++i) lst.push_back(i);

    int expected = 1;
    for (auto it = lst.begin(); it != lst.end(); ++it, ++expected) {
        assert(*it == expected);
    }

    expected = 5;
    for (auto it = --lst.end(); it != --lst.begin(); --it, --expected) {
        assert(*it == expected);
    }
    std::cout << "test_iterator_access() success!\n";
}

void test_insert_erase() {
    my::list<int> lst;
    for (int i = 1; i <= 3; ++i) lst.push_back(i); // 1 2 3
    auto it = lst.begin(); ++it; // 指向 2
    lst.insert(it, 99); // 插入前: 1 99 2 3
    assert(*++lst.begin() == 99);

    it = lst.begin(); ++it; // 再次指向 99
    lst.erase(it); // 删除: 1 2 3
    int expected = 1;
    for (auto i : lst) assert(i == expected++);
    std::cout << "test_insert_erase() success!\n";
}

void test_remove_unique() {
    my::list<int> lst = {1, 2, 2, 3, 3, 3, 4};
    lst.unique(); // 删除连续重复值
    std::vector<int> expected = {1, 2, 3, 4};
    auto it = lst.begin();
    for (auto val : expected) {
        assert(*it == val);
        ++it;
    }

    lst.push_back(99);
    lst.push_back(99);
    lst.remove(99);
    assert(lst.back() == 4);
    std::cout << "test_remove_unique() success!\n";
}

void test_resize_clear() {
    my::list<int> lst = {1, 2, 3};
    lst.resize(5, 9);
    assert(lst.size() == 5);
    auto it = lst.begin(); std::advance(it, 3);
    assert(*it == 9);

    lst.resize(2);
    assert(lst.size() == 2);
    lst.clear();
    assert(lst.empty());
    std::cout << "test_resize_clear() success!\n";
}

void test_splice_merge() {
    std::cout << "Creating my::list a = {1, 3, 5}\n";
    my::list<int> a = {1, 3, 5};
    std::cout << "Creating my::list b = {2, 4, 6}\n";
    my::list<int> b = {2, 4, 6};

    std::cout << "a before splice: ";
    for (auto v : a) std::cout << v << ' ';
    std::cout << "\nb before splice: ";
    for (auto v : b) std::cout << v << ' ';
    std::cout << std::endl;

    auto it = a.begin();
    ++it;  // 指向 a 的第二个元素 (3)

    std::cout << "Splicing b into a at position 2 (before element 3)\n";
    a.splice(it, b);

    std::cout << "a after splice: ";
    for (auto v : a) std::cout << v << ' ';
    std::cout << "\nb after splice (should be empty): ";
    for (auto v : b) std::cout << v << ' ';
    std::cout << "\nempty? " << (b.empty() ? "yes" : "no") << std::endl;

    std::vector<int> expected = {1, 2, 4, 6, 3, 5};
    auto ait = a.begin();
    for (int val : expected) {
        std::cout << "Checking a element, expected: " << val << ", actual: " << *ait << std::endl;
        assert(*ait++ == val);
    }

    std::cout << "Sorting a\n";
    a.sort();

    std::cout << "Resetting b = {0, 7, 8}\n";
    b = {0, 7, 8};

    std::cout << "a before merge: ";
    for (auto v : a) std::cout << v << ' ';
    std::cout << "\nb before merge: ";
    for (auto v : b) std::cout << v << ' ';
    std::cout << std::endl;

    std::cout << "Merging b into a\n";
    a.merge(b);

    std::cout << "a after merge: ";
    for (auto v : a) std::cout << v << ' ';
    std::cout << "\nb after merge (should be empty): ";
    for (auto v : b) std::cout << v << ' ';
    std::cout << "\nempty? " << (b.empty() ? "yes" : "no") << std::endl;

    std::vector<int> merged = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    ait = a.begin();
    for (int val : merged) {
        std::cout << "Checking a element, expected: " << val << ", actual: " << *ait << std::endl;
        assert(*ait++ == val);
    }

    std::cout << "test_splice_merge() success!\n";
}


void test_operator_equal_swap() {
    my::list<std::string> a;
    my::list<std::string> b;
// 添加一些元素
    a.push_back("hi");
    b.push_back("hi");

    assert(a == b); // 要用到了 const_iterator


    b.push_back("d");
    assert(!(a == b));

    a.swap(b);
    assert(a.back() == "d");
    std::cout << "test_operator_equal_swap() success!\n";
}

void print_list(const my::list<int>& lst) {
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "test_iterator_access() success!\n";
}


#endif
int main() {
    test_basic_insert_pop();
    test_iterator_access();
    test_insert_erase();
    test_remove_unique();
    test_resize_clear();
    test_splice_merge();
    test_operator_equal_swap();
    std::cout << "All my::list tests passed!" << std::endl;

#if 0
    my::list<int> lst = {1, 3, 5, 7, 9};
    lst.insert(lst.begin(), 0);     // 前插
    lst.insert(lst.end(), 10);      // 后插
    print_my::list(lst);
    lst.push_front(-1);
    lst.push_back(11);
    print_my::list(lst);
    lst.pop_front();
    lst.pop_back();
    print_my::list(lst);

    my::list<int> dup = {1, 2, 2, 3, 3, 3, 4};
    dup.unique();  // 去重测试
    print_my::list(dup);

    my::list<int> rm = {1, 3, 5, 7, 9};
    rm.remove_if([](int x){ return x % 3 == 0; });
    print_my::list(rm);

    my::list<int> to_sort = {5, 2, 9, 1, 3};
    to_sort.sort();  // 排序测试
    print_my::list(to_sort);
//
#endif
    return 0;
}