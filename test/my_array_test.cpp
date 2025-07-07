/**
 * @file      my_array_test.cpp
 * @brief     [arrsy_test]
 * @author    Weijh
 * @date      2025/7/5
 * @version   1.0
 */

#include "my_array.h"
#include <iostream>
#include <array>

void test(std::array<int, 5>& arr)
{
    for (size_t i = 0; i < arr.size(); i++) {
        arr[i] *= 2;
    }
}

void test2(my::array<int, 5>& arr)
{
    for (size_t i = 0; i < arr.size(); i++) {
        arr[i] *= 2;
    }
}

int main()
{
#if 1
    // 先用标准array库完成
    std::cout << "this is stl::array" << std::endl;
    int arr[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i  < 5; i++)
    {
        std::cout << *(arr + i) << " ";
    }
    std::cout << std::endl;

    std::array<int, 5> arr2 = {1, 2, 3, 4, 5};
    for (int num : arr2) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::array<int, 5> arr3 = {1, 2, 3, 4, 5};
    test(arr3);
    for (int num : arr3) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "this is MyArray" << std::endl;
    my::array<int, 5> a1 = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < 5; i++) {
        std::cout << a1[i] << " ";
    }
    std::cout << std::endl;
    test2(a1);
    for (size_t i = 0; i < 5; i++) {
        std::cout << a1[i] << " ";
    }
    std::cout << std::endl;

    /// 测试访问下标越界
    std::cout << a1.at(5) << std::endl;
    /// terminate called after throwing an instance of 'std::out_of_range'
    //  what():  array::at
    std::cout << a1[2] << std::endl;

    /**
        this is stl::array
        1 2 3 4 5
        1 2 3 4 5
        2 4 6 8 10
        this is MyArray
        1 2 3 4 5
        2 4 6 8 10
     * */
#else
    my::array<int, 5> arr;
    arr.fill(10);
    arr[2] = 99;

    std::cout << "test for:";
    for (auto it = arr.begin(); it != arr.end(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    std::cout << "reverse test:";
    for (auto it = arr.rbegin(); it != arr.rend(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';

    std::cout << "random test:" << arr.begin()[2] << '\n';
    /**
            test for:10 10 99 10 10
            reverse test:10 10 99 10 10
            random test:99
 * */
#endif
    return 0;
}

