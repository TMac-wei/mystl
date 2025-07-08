/**
 * @file      my_multiset_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_multiset.h"
#include <vector>
#include <iostream>

int main() {
    my::multiset<int> ms;

    ms.insert(5);
    ms.insert(2);
    ms.insert(5);
    ms.insert(1);
    ms.insert(2);
    ms.insert(3);

    std::cout << "Multiset inorder traversal: ";
    ms.inorder();  /// 应为 1 2 2 3 5 5

    std::cout << "Count of 2: " << ms.count(2) << std::endl;
    std::cout << "Count of 5: " << ms.count(5) << std::endl;
    std::cout << "Count of 4: " << ms.count(4) << std::endl;

    std::cout << "Size: " << ms.size() << std::endl;

    if (auto it = ms.find(3); it != ms.end()) {
        std::cout << "Found 3 at: " << *it << std::endl;
    }

    /**
        Multiset inorder traversal: 1 2 2 3 5 5
        Count of 2: 2
        Count of 5: 2
        Count of 4: 0
        Size: 6
        Found 3 at: 3
     * */
}

