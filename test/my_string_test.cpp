/**
 * @file      my_string_test.cpp
 * @brief     [string test]
 * @author    Weijh
 * @date      2025/7/4
 * @version   1.0
 */

#include <cassert>
#include <iostream>
#include "my_string.h"

void test_basic_operations() {
    using my::string;

    // 构造与赋值
    string s1("hello");
    assert(s1.size() == 5);
    assert(s1 == "hello");

    // 拷贝构造
    string s2(s1);
    assert(s2 == s1);

    // 移动构造
    string s3(std::move(s2));
    assert(s3 == "hello");
    assert(s2.empty());

    // 单字符构造
    string s4(5, 'x');
    assert(s4 == "xxxxx");

    // assign
    s4.assign(3, 'z');
    assert(s4 == "zzz");

    s4.assign("world", 5);
    assert(s4 == "world");

    s4.assign("abc");
    assert(s4 == "abc");

    // assign 移动赋值
    string s5("temp");
    s4.assign(std::move(s5));
    assert(s4 == "temp");

    // append
    s4.append("123", 3);
    std::cout << "s4 = [" << s4.data() << "], size = " << s4.size() << ", strlen = " << strlen(s4.data()) << '\n';
    assert(s4 == "temp123");

    s4.append(string("XYZ"));
    std::cout << "s4 = [" << s4.data() << "], size = " << s4.size() << ", strlen = " << strlen(s4.data()) << '\n';
    assert(s4 == "temp123XYZ");

    // push_back
    s4.push_back('!');
    std::cout << "s4 = [" << s4.data() << "], size = " << s4.size() << ", strlen = " << strlen(s4.data()) << '\n';
    assert(s4 == "temp123XYZ!");

    // resize
    s4.resize(5);   // shrink
    assert(s4 == "temp1");

    s4.resize(10);  // grow
    assert(s4.size() == 10);
    assert(s4[5] == '\0');

    // clear
    s4.clear();
    assert(s4.empty());

    std::cout << "[ok]test_basic_operations passed." << std::endl;
}

void test_sso_behavior() {
    using my::string;

    string s1("abc");     // fits in SSO
    assert(s1.size() == 3);
    assert(s1 == "abc");

    string s2(100, 'x');  // heap allocation
    assert(s2.size() == 100);
    assert(s2[0] == 'x');
    assert(s2[99] == 'x');

    std::cout << "[ok]test_sso_behavior passed." << std::endl;
}

void test_relational_operators() {
    using my::string;

    string s1("hello");
    string s2("hello");
    string s3("world");

    assert(s1 == s2);
    assert(!(s1 != s2));
    assert(s1 != s3);
    assert(s1 == "hello");
    assert("hello" == s1);
    assert("world" != s1);

    std::cout << "[ok]test_relational_operators passed." << std::endl;
}

int main() {
    test_basic_operations();
    test_sso_behavior();
    test_relational_operators();

    std::cout << "[ok]All my::string tests passed!" << std::endl;
    return 0;
}
