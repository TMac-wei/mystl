/**
 * @file      my_vector_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#include <iostream>
#include <cassert>
#include <string>
#include "./my_vector.h"

using namespace my;

struct MyStruct {
    int x;
    std::string y;

    MyStruct(int x_, std::string y_) : x(x_), y(std::move(y_)) {
        std::cout << "Construct MyStruct(" << x << ", " << y << ")\n";
    }

    MyStruct(const MyStruct& other) : x(other.x), y(other.y) {
        std::cout << "Copy MyStruct(" << x << ", " << y << ")\n";
    }

    MyStruct(MyStruct&& other) noexcept : x(other.x), y(std::move(other.y)) {
        std::cout << "Move MyStruct(" << x << ", " << y << ")\n";
    }

    MyStruct& operator=(const MyStruct& other) = default;
    MyStruct& operator=(MyStruct&& other) noexcept = default;

    bool operator==(const MyStruct& other) const {
        return x == other.x && y == other.y;
    }
};

void test_initializer_list() {
    vector<int> v = {1, 2, 3, 4};
    assert(v.size() == 4);
    assert(v[2] == 3);

    v = {5, 6};
    assert(v.size() == 2);
    assert(v[1] == 6);
    std::cout << "test_initializer_list passed.\n";
}

void test_assign_range() {
    vector<int> v;
    int arr[] = {10, 20, 30, 40};
    v.assign(arr, arr + 4);
    assert(v.size() == 4);
    assert(v[0] == 10 && v[3] == 40);
    std::cout << "test_assign_range passed.\n";
}

void test_insert_range() {
    vector<int> v = {1, 2, 6};
    int arr[] = {3, 4, 5};
    v.insert(v.begin() + 2, arr, arr + 3);  // 1 2 3 4 5 6
    for (int i = 0; i < 6; ++i)
        assert(v[i] == i + 1);
    std::cout << "test_insert_range passed.\n";
}

void test_emplace() {
    vector<MyStruct> v;
    v.emplace_back(1, "hello");
    v.emplace_back(2, "world");
    assert(v.size() == 2);
    assert(v[1] == MyStruct(2, "world"));

    v.emplace(v.begin() + 1, 99, "middle");
    assert(v[1].x == 99 && v[1].y == "middle");
    std::cout << "test_emplace passed.\n";
}

void test_allocator_traits_integration() {
    vector<std::string> v(3, "init");
    v.emplace_back("newstring");
    assert(v.back() == "newstring");
    v.assign(2, "assigned");
    assert(v.size() == 2 && v[1] == "assigned");
    std::cout << "test_allocator_traits_integration passed.\n";
}

int main() {
    test_initializer_list();
    test_assign_range();
    test_insert_range();
    test_emplace();
    test_allocator_traits_integration();
    std::cout << "All enhanced vector<T> tests passed.\n";
    return 0;
}
