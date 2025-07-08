/**
 * @file      my_optional_test.cpp
 * @brief     [optional_test]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_optional.h"
#include <cassert>
#include <string>
#include <iostream>

struct MyTest {
    MyTest(int x = 0) : x(x) {}

    MyTest(const MyTest &other) : x(other.x) {}

    MyTest(MyTest &&other) noexcept: x(other.x) { other.x = -1; }

    MyTest &operator=(const MyTest &other) {
        x = other.x;
        return *this;
    }

    MyTest &operator=(MyTest &&other) noexcept {
        x = other.x;
        other.x = -1;
        return *this;
    }

    ~MyTest() = default;

    int x;
};

void test_optional_basic() {
    using my::optional;
    using my::nullopt;

    optional<int> o1;
    assert(!o1.has_value());

    o1 = 42;
    assert(o1.has_value());
    assert(o1.value() == 42);

    optional<int> o2 = o1;
    assert(o2.has_value());
    assert(o2.value() == 42);

    o2 = nullopt;
    assert(!o2.has_value());

    optional<std::string> o3("hello");
    assert(o3.has_value());
    assert(*o3 == "hello");

    o3 = std::string("world");
    assert(o3.value() == "world");

    optional<MyTest> o4{MyTest(5)};
    assert(o4.has_value());
    assert(o4->x == 5);

    optional<MyTest> o5 = std::move(o4);
    assert(o5.has_value());
    assert(o5->x == 5);
}

int main() {
    test_optional_basic();
    std::cout << "All tests passed!" << std::endl;
}



