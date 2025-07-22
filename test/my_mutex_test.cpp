/**
 * @file      my_mutex_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_mutex.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

/// 全局共享资源（用于验证锁的正确性）
std::atomic<int> shared_value = 0;
const int LOOP_COUNT = 5;  /// 每个线程操作共享资源的次数


/// ------------------------------ 1. 测试普通mutex + lock_guard/unique_lock ------------------------------
void test_mutex_basic() {
    std::cout << "\n=== 测试1：普通mutex + lock_guard/unique_lock 基础功能 ===" << std::endl;
    my::mutex mtx;
    shared_value = 0;

    // 线程函数：使用lock_guard保护共享资源
    auto func_lock_guard = [&]() {
        for (int i = 0; i < LOOP_COUNT; ++i) {
            std::cout << "线程" << std::this_thread::get_id() << "：尝试获取lock_guard" << std::endl;
            my::lock_guard<my::mutex> lock(mtx);  // RAII自动加锁
            std::cout << "线程" << std::this_thread::get_id() << "：成功获取lock_guard，修改shared_value（当前值：" << shared_value << "）" << std::endl;
            shared_value++;  // 操作共享资源
            std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 模拟处理时间
            // lock_guard析构时自动释放锁
            std::cout << "线程" << std::this_thread::get_id() << "：lock_guard自动释放锁，shared_value更新为：" << shared_value << std::endl;
        }
    };

    // 线程函数：使用unique_lock保护共享资源
    auto func_unique_lock = [&]() {
        for (int i = 0; i < LOOP_COUNT; ++i) {
            std::cout << "线程" << std::this_thread::get_id() << "：尝试获取unique_lock" << std::endl;
            my::unique_lock<my::mutex> lock(mtx);  // 手动加锁
            std::cout << "线程" << std::this_thread::get_id() << "：成功获取unique_lock，修改shared_value（当前值：" << shared_value << "）" << std::endl;
            shared_value++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            lock.unlock();  // 手动释放锁（也可依赖析构自动释放）
            std::cout << "线程" << std::this_thread::get_id() << "：unique_lock手动释放锁，shared_value更新为：" << shared_value << std::endl;
        }
    };

    // 创建2个线程（分别使用lock_guard和unique_lock）
    std::thread t1(func_lock_guard);
    std::thread t2(func_unique_lock);
    t1.join();
    t2.join();

    // 验证结果：2个线程各操作5次，预期值为10
    std::cout << "测试1结果：shared_value最终值 = " << shared_value
              << "（预期值：" << 2 * LOOP_COUNT << "）" << (shared_value == 2 * LOOP_COUNT ? " [通过]" : " [失败]") << std::endl;
}


// ------------------------------ 2. 测试recursive_mutex（递归锁） ------------------------------
void recursive_func(my::recursive_mutex& rmtx, int depth) {
    if (depth <= 0) return;

    // 同一线程多次获取递归锁
    std::cout << "线程" << std::this_thread::get_id() << "：递归深度" << depth << "，尝试获取递归锁" << std::endl;
    my::lock_guard<my::recursive_mutex> lock(rmtx);  // 第1次获取
    std::cout << "线程" << std::this_thread::get_id() << "：递归深度" << depth << "，成功获取递归锁" << std::endl;

    // 递归调用（同一线程再次获取锁）
    shared_value++;
    recursive_func(rmtx, depth - 1);

    std::cout << "线程" << std::this_thread::get_id() << "：递归深度" << depth << "，释放递归锁" << std::endl;
}

void test_recursive_mutex() {
    std::cout << "\n=== 测试2：recursive_mutex（递归锁）功能 ===" << std::endl;
    my::recursive_mutex rmtx;
    shared_value = 0;

    // 线程函数：测试递归获取锁
    auto func = [&]() {
        recursive_func(rmtx, 3);  // 递归3层（同一线程获取3次锁）
    };

    std::thread t1(func);
    t1.join();

    // 验证结果：递归3次，shared_value应增加3
    std::cout << "测试2结果：shared_value最终值 = " << shared_value
              << "（预期值：3）" << (shared_value == 3 ? " [通过]" : " [失败]") << std::endl;
}


// ------------------------------ 3. 测试timed_mutex（带超时的互斥锁） ------------------------------
void test_timed_mutex() {
    std::cout << "\n=== 测试3：timed_mutex（超时锁）功能 ===" << std::endl;
    my::timed_mutex tmtx;
    shared_value = 0;

    // 线程1：持有锁500ms，模拟长时间操作
    auto hold_lock = [&]() {
        std::cout << "线程A：尝试获取timed_mutex" << std::endl;
        my::unique_lock<my::timed_mutex> lock(tmtx);
        std::cout << "线程A：成功获取锁，开始持有500ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 持有锁500ms
        shared_value = 100;  // 标记线程A操作完成
        std::cout << "线程A：释放锁" << std::endl;
    };

    // 线程2：尝试在200ms内获取锁（预期超时）
    auto try_timeout = [&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 确保线程A先获取锁
        std::cout << "线程B：尝试在200ms内获取锁" << std::endl;
        bool success = tmtx.try_lock_for(std::chrono::milliseconds(200));
        if (success) {
            std::cout << "线程B：超时前获取锁（错误！）" << std::endl;
            tmtx.unlock();
        } else {
            std::cout << "线程B：200ms超时，未获取锁（正确）" << std::endl;
        }
    };

    // 线程3：尝试在600ms内获取锁（预期成功）
    auto try_success = [&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 确保线程A先获取锁
        std::cout << "线程C：尝试在600ms内获取锁" << std::endl;
        bool success = tmtx.try_lock_for(std::chrono::milliseconds(600));
        if (success) {
            std::cout << "线程C：超时前获取锁（正确）" << std::endl;
            shared_value = 200;  // 标记线程C操作完成
            tmtx.unlock();
        } else {
            std::cout << "线程C：600ms超时（错误！）" << std::endl;
        }
    };

    std::thread t1(hold_lock);
    std::thread t2(try_timeout);
    std::thread t3(try_success);
    t1.join();
    t2.join();
    t3.join();

    // 验证结果：线程C应成功修改值为200
    std::cout << "测试3结果：shared_value最终值 = " << shared_value
              << "（预期值：200）" << (shared_value == 200 ? " [通过]" : " [失败]") << std::endl;
}


// ------------------------------ 4. 测试shared_mutex（共享锁） ------------------------------
void test_shared_mutex() {
    std::cout << "\n=== 测试4：shared_mutex（读写锁）功能 ===" << std::endl;
    my::shared_mutex smtx;
    shared_value = 0;

    // 读线程函数：共享获取锁，读取共享资源
    auto read_func = [&](int id) {
        for (int i = 0; i < LOOP_COUNT; ++i) {
            std::cout << "读线程" << id << "：尝试获取共享锁" << std::endl;
            my::shared_lock<my::shared_mutex> lock(smtx);  // 共享锁
            std::cout << "读线程" << id << "：获取共享锁，当前shared_value = " << shared_value << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟读操作
            std::cout << "读线程" << id << "：释放共享锁" << std::endl;
        }
    };

    // 写线程函数：独占获取锁，修改共享资源
    auto write_func = [&](int id) {
        for (int i = 0; i < 2; ++i) {  // 写操作次数少一些
            std::cout << "写线程" << id << "：尝试获取独占锁" << std::endl;
            my::unique_lock<my::shared_mutex> lock(smtx);  // 独占锁
            std::cout << "写线程" << id << "：获取独占锁，修改shared_value（旧值：" << shared_value << "）" << std::endl;
            shared_value += 10;  // 每次写+10
            std::this_thread::sleep_for(std::chrono::milliseconds(300));  // 模拟写操作
            std::cout << "写线程" << id << "：释放独占锁（新值：" << shared_value << "）" << std::endl;
        }
    };

    // 创建3个读线程和2个写线程
    std::thread r1(read_func, 1);
    std::thread r2(read_func, 2);
    std::thread r3(read_func, 3);
    std::thread w1(write_func, 1);
    std::thread w2(write_func, 2);

    r1.join();
    r2.join();
    r3.join();
    w1.join();
    w2.join();

    // 验证结果：2个写线程各写2次（每次+10），最终值应为40
    std::cout << "测试4结果：shared_value最终值 = " << shared_value
              << "（预期值：40）" << (shared_value == 40 ? " [通过]" : " [失败]") << std::endl;
}


// ------------------------------ 5. 测试condition_variable（条件变量） ------------------------------
my::mutex cond_mtx;
my::condition_variable cond;
bool data_ready = false;  // 条件变量的判断条件

void cond_waiter() {
    std::cout << "等待线程：尝试获取锁" << std::endl;
    my::unique_lock<my::mutex> lock(cond_mtx);
    std::cout << "等待线程：获取锁，等待数据准备" << std::endl;

    // 等待条件满足（data_ready == true）
    cond.wait(lock, []() { return data_ready; });

    std::cout << "等待线程：条件满足，处理数据（shared_value = " << shared_value << "）" << std::endl;
    shared_value.store(shared_value.load() * 2); // 处理数据
    std::cout << "等待线程：处理完成（shared_value = " << shared_value << "）" << std::endl;
}

void cond_notifier() {
    std::cout << "通知线程：尝试获取锁" << std::endl;
    my::lock_guard<my::mutex> lock(cond_mtx);
    std::cout << "通知线程：获取锁，准备数据" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 模拟数据准备
    shared_value = 5;  // 准备数据
    data_ready = true;  // 标记条件满足
    std::cout << "通知线程：数据准备完成，通知等待线程" << std::endl;

    cond.notify_one();  // 通知等待线程
}

void test_condition_variable() {
    std::cout << "\n=== 测试5：condition_variable（条件变量）功能 ===" << std::endl;
    shared_value = 0;
    data_ready = false;

    std::thread waiter(cond_waiter);
    std::thread notifier(cond_notifier);
    waiter.join();
    notifier.join();

    // 验证结果：等待线程应将数据5处理为10
    std::cout << "测试5结果：shared_value最终值 = " << shared_value
              << "（预期值：10）" << (shared_value == 10 ? " [通过]" : " [失败]") << std::endl;
}


// 主函数：执行所有测试
int main() {
    system("chcp 65001");
    std::cout << "===== 开始锁组件综合测试 =====" << std::endl;

    test_mutex_basic();       // 测试普通锁+锁守卫
    test_recursive_mutex();   // 测试递归锁
    test_timed_mutex();       // 测试带超时的锁
    test_shared_mutex();      // 测试共享锁（读写锁）
    test_condition_variable();// 测试条件变量

    std::cout << "\n===== 所有测试结束 =====" << std::endl;
    return 0;
}
