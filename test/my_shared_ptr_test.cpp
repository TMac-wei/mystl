/**
 * @file      my_shared_ptr_test.cpp
 * @brief     [shared_ptr test]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_shared_ptr.h"

#include <iostream>
#include <cassert>

using namespace my;

struct Base {
    virtual ~Base() { std::cout << "Base dtor\n"; }
    virtual void hello() const { std::cout << "Base::hello\n"; }
};

struct Derived : public Base {
    ~Derived() { std::cout << "Derived dtor\n"; }
    void hello() const override { std::cout << "Derived::hello\n"; }
};

void test_shared_ptr_basic() {
    shared_ptr<int> sp1(new int(42));
    assert(sp1.use_count() == 1);
    shared_ptr<int> sp2 = sp1;
    assert(sp1.use_count() == 2);
    assert(*sp2 == 42);
    sp1 = nullptr;      /// 需要为shared_ptr实现一个可接受的nullptr的赋值运算符
    assert(sp2.use_count() == 1);
    sp2 = nullptr; // 应该释放资源

    std::cout << "test_shared_ptr_basic success!" << std::endl;
}

void test_make_shared() {
    auto sp = make_shared<std::pair<int, int>>(1, 2);
    assert(sp->first == 1);
    assert(sp.use_count() == 1);

    std::cout << "test_make_shared success!" << std::endl;
}

void test_type_conversion() {
    shared_ptr<Derived> dp = make_shared<Derived>();
    shared_ptr<Base> bp = static_pointer_cast<Base>(dp);
    bp->hello();

    shared_ptr<Base> bp2 = dp;
    shared_ptr<Derived> dp2 = dynamic_pointer_cast<Derived>(bp2);
    assert(dp2); // dynamic cast 成功

    std::cout << "test_type_conversion success!" << std::endl;
}

void test_weak_ptr() {
    shared_ptr<int> sp = make_shared<int>(123);
    weak_ptr<int> wp(sp);
    assert(wp.use_count() == sp.use_count());
    assert(!wp.expired());
    {
        shared_ptr<int> sp2 = wp.lock();
        assert(sp2);
        assert(*sp2 == 123);
    }
    sp = nullptr;
    /// 打印use_count验证是否在sp=nullptr之后，还有别的sp没有被析构
//    std::cout << "use_count after sp = nullptr: " << wp.use_count() << std::endl;
//    std::cout << "wp.lock().get(): " << wp.lock().get() << std::endl;

    assert(wp.expired());
    /// 打印use_count验证是否在sp=nullptr之后，还有别的sp没有被析构
//    std::cout << "use_count after sp = nullptr: " << wp.use_count() << std::endl;
//    std::cout << "wp.lock().get(): " << wp.lock().get() << std::endl;
    assert(!wp.lock());

    std::cout << "test_weak_ptr success!" << std::endl;
}

int main() {
    test_shared_ptr_basic();
    test_make_shared();
    test_type_conversion();
    test_weak_ptr();
    std::cout << "All tests passed.\n";

    /**
        test_shared_ptr_basic success!
        test_make_shared success!
        Derived::hello
        test_type_conversion success!
        Derived dtor
        Base dtor
        test_weak_ptr success!
        All tests passed.

        进程已结束，退出代码为 0
     * */
}

