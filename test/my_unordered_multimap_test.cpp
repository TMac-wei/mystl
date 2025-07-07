/**
 * @file      my_unordered_multimap_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @date      2025/7/6
 * @version   1.0
 */

#include "my_unordered_multimap.h"
#include <iostream>

int main() {
    my::unordered_multimap<std::string, int> mmap = {
            {"apple", 1},
            {"banana", 2},
            {"apple", 3},
            {"apple", 4},
            {"banana", 5},
            {"cherry", 6}
    };

    std::cout << "== All Elements:\n";
    for (const auto& p : mmap) {
        std::cout << p.first << " => " << p.second << "\n";
    }

    std::cout << "\n== Count of 'apple': " << mmap.count("apple") << "\n";

    std::cout << "\n== Equal Range for 'apple':\n";
    auto range = mmap.equal_range("apple");
    for (auto it = range.first_; it != range.second_; ++it) {
        std::cout << it->first << " => " << it->second << "\n";
    }

    std::cout << "\n== Find 'banana': ";
    auto it = mmap.find("banana");
    if (it != mmap.end())
        std::cout << it->first << " => " << it->second << "\n";

    std::cout << "\n== Erase 'banana': " << mmap.erase("banana") << " removed\n";

    std::cout << "\n== After erase:\n";
    for (const auto& p : mmap) {
        std::cout << p.first << " => " << p.second << "\n";
    }

    std::cout << "\n== Clear all\n";
    mmap.clear();
    std::cout << "Empty? " << std::boolalpha << mmap.empty() << "\n";

    /**
            == All Elements:
            cherry => 6
            banana => 5
            banana => 2
            apple => 4
            apple => 3
            apple => 1

            == Count of 'apple': 3

            == Equal Range for 'apple':
            apple => 4
            apple => 3
            apple => 1

            == Find 'banana': banana => 5

            == Erase 'banana': 2 removed

            == After erase:
            cherry => 6
            apple => 4
            apple => 3
            apple => 1

            == Clear all
            Empty? true
     * */

    return 0;
}

