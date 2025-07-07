/**
 * @file      my_deque_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "my_deque.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <memory>

int main()
{
    constexpr size_t chunk_size = 10;
    constexpr size_t chunk_count = 6;

    // 创建 map（chunk 指针数组）
    using T = int;
    using iterator = my::deque_iterator<T, T&, T*>;
    std::vector<std::unique_ptr<T[]>> chunks;

    T** map = new T*[chunk_count];
    int val = 0;

    for (size_t i = 0; i < chunk_count; ++i) {
        chunks.emplace_back(new T[chunk_size]);
        for (size_t j = 0; j < chunk_size; ++j)
            chunks[i][j] = val++;
        map[i] = chunks[i].get();
    }

    // 创建两个迭代器，指向 map[0][1] 和 map[1][2]
    iterator it1(map[0] + 1, map + 0, chunk_size);
    iterator it2(map[1] + 2, map + 1, chunk_size);

    std::cout << "*it1 = " << *it1 << ", *it2 = " << *it2 << std::endl;

    std::cout << "Distance: it2 - it1 = " << (it2 - it1) << std::endl;

    // 测试 ++ --
    std::cout << "++it1: " << *(++it1) << std::endl;
    std::cout << "--it2: " << *(--it2) << std::endl;

    // 跨 chunk 测试
    iterator it3(map[0] + 3, map + 0, chunk_size); // 最后一个元素
    ++it3; // 应该跳到 map[1][0]
    std::cout << "After ++it3: " << *it3 << std::endl;

    // 测试 +=
    iterator it4(map[0], map + 0, chunk_size);
    it4 += 5;  // 应该跳到 map[1][1]
    std::cout << "it4 += 5: " << *it4 << std::endl;

    // 测试 operator[]
    std::cout << "it4[-1]: " << it4[-1] << std::endl; // 应该是 map[1][0]

    delete[] map;
    std::cout << "Test passed.\n";

    /**
        *it1 = 1, *it2 = 6
        Distance: it2 - it1 = 5
        ++it1: 2
        --it2: 5
        After ++it3: 4
        operator+= cross node: offset=5 node_offset=1
        it4 += 5: 5
        it4[-1]: 4
        Test passed.
     * */


}
