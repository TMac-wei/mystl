/**
 * @file      my_unique_ptr_test.cpp
 * @brief     [my_unique_ptr_test]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_unique_ptr.h"
#include <iostream>
#include <memory>

/// 基本使用和析构
struct Foo {
    Foo(int x) : x(x) {}
    ~Foo() { std::cout << "Foo " << x << " destroyed\n"; }
    int x;
};

void test_basic() {
    my::unique_ptr<Foo> ptr(new Foo(42));
    std::cout << ptr->x << std::endl;
    std::cout << "test_basic success!!"<< std::endl;

}

/// release()转交资源
void test_release() {
    my::unique_ptr<Foo> ptr(new Foo(10));
    Foo* raw = ptr.release();
    std::cout << raw->x << std::endl;
    delete raw; /// 手动释放
    std::cout << "test_release success!!"<< std::endl;
}

/// reset() 与自删除保
void test_reset()
{
    my::unique_ptr<Foo> ptr(new Foo(20));
    Foo* same = ptr.get();          /// 将ptr中的指针获取赋给same
    ptr.reset(same);            /// 自己删除自己测试，正常情况不应该崩溃
    std::cout << "test_reset success!!"<< std::endl;
}

/// 测试数组的情况
void test_array()
{
    my::unique_ptr<int[]> arr(new int[5]{1, 2, 3, 4, 5});
    for (size_t i = 0; i < 5; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "test_array success!!"<< std::endl;
}

int main()
{
    test_basic(); // 42  Foo 42 destroyed
    test_release();
    test_reset();
    test_array();
    /**
        42
        test_basic success!!
        Foo 42 destroyed
        10
        Foo 10 destroyed
        test_release success!!
        test_reset success!!
        Foo 20 destroyed
        1 2 3 4 5
        test_array success!!

进程已结束，退出代码为 0
     * */


    return 0;
}
