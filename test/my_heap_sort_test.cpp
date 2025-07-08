/**
 * @file      my_heap_sort_test.cpp
 * @brief     [heap_sort_test]
 * @author    Weijh
 * @version   1.0
 */

#include "my_heap_sort.h"

#include <algorithm>
#include <cassert>

/// 基本堆操作测试
void test_basic_heap() {
    std::vector<int> v = {3, 1, 4, 2, 5};
    my::make_heap(v.begin(), v.end());
    assert(std::is_heap(v.begin(), v.end()));

    v.push_back(6);
    my::push_heap(v.begin(), v.end());
    assert(std::is_heap(v.begin(), v.end()));

    my::pop_heap(v.begin(), v.end());
    v.pop_back();
    assert(std::is_heap(v.begin(), v.end()));

    std::cout << "test_basic_heap() success!\n";
}


/// 自定义比较器（大顶堆）
void test_custom_comparator() {
    std::vector<int> v = {4, 5, 3, 1, 2};
    my::make_heap(v.begin(), v.end(), std::greater<int>());
    assert(std::is_heap(v.begin(), v.end(), std::greater<int>()));

    std::cout << "test_custom_comparator() success!\n";
}

/// 指针型迭代器测试（检验 iterator_traits）
void test_raw_pointer() {
    int arr[] = {10, 30, 20, 5, 40};
    my::make_heap(arr, arr + 5);
    assert(std::is_heap(arr, arr + 5));
    std::cout << "test_raw_pointer() success!\n";
}

#if 1
/// 排序测试
void test_sort_heap() {
    std::vector<int> v = {9, 3, 7, 1, 5};
    my::make_heap(v.begin(), v.end());
    my::sort_heap(v.begin(), v.end());
    assert(std::is_sorted(v.begin(), v.end())); // 升序
    std::cout << "test_sort_heap() success!\n";

}
#endif
int main()
{
    test_basic_heap();
    test_custom_comparator();
    test_raw_pointer();
    test_sort_heap();

    /**
        test_basic_heap() success!
        test_custom_comparator() success!
        test_raw_pointer() success!
        test_sort_heap() success!

进程已结束，退出代码为 0
     * */
}