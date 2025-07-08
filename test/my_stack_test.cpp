/**
 * @file      my_stack_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_stack.h"

#include <iostream>
#include <string>


void test_basic_stack() {
    std::cout << "== 基本功能测试 ==" << std::endl;
    my::stack<int> s;
    std::cout << "初始是否为空: " << std::boolalpha << s.empty() << std::endl;

    s.push(10);
    s.push(20);
    s.push(30);

    std::cout << "当前栈顶元素: " << s.top() << std::endl;
    std::cout << "栈大小: " << s.size() << std::endl;

    s.pop();
    std::cout << "出栈后栈顶元素: " << s.top() << std::endl;
    std::cout << "出栈后栈大小: " << s.size() << std::endl;

    while (!s.empty()) {
        std::cout << "弹出元素: " << s.top() << std::endl;
        s.pop();
    }

    std::cout << "栈清空后是否为空: " << s.empty() << std::endl;
    std::cout << "test_basic_stack success!\n ";
}

void test_emplace_stack() {
    std::cout << "\n== emplace 测试 ==" << std::endl;

    my::stack<std::string> s;
    s.emplace(5, 'a'); // 创建 "aaaaa"
    s.emplace("hello");
    s.emplace("world");

    while (!s.empty()) {
        std::cout << s.top() << std::endl;
        s.pop();
    }
    std::cout << "test_emplace_stack success!\n ";
}

void test_stack_swap() {
    std::cout << "\n== swap 测试 ==" << std::endl;

    my::stack<int> s1;
    my::stack<int> s2;

    s1.push(1);
    s1.push(2);
    s2.push(100);
    s2.push(200);
    s2.push(300);

    std::cout << "交换前：" << std::endl;
    std::cout << "s1 栈顶: " << s1.top() << ", s1 大小: " << s1.size() << std::endl;
    std::cout << "s2 栈顶: " << s2.top() << ", s2 大小: " << s2.size() << std::endl;

    s1.swap(s2);

    std::cout << "交换后：" << std::endl;
    std::cout << "s1 栈顶: " << s1.top() << ", s1 大小: " << s1.size() << std::endl;
    std::cout << "s2 栈顶: " << s2.top() << ", s2 大小: " << s2.size() << std::endl;

    std::cout << "test_stack_swap success!\n ";
}

int main() {
    system("chcp 65001");
    test_basic_stack();
    test_emplace_stack();
    test_stack_swap();
    return 0;
}


