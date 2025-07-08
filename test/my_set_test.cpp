/**
 * @file      my_set_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_set.h"
#include <vector>

int main() {
    my::set<int> s;
    s.insert(3);
    s.insert(1);
    s.insert(4);
    s.insert(1); /// 重复元素，应该无效
    s.insert(2);

    std::cout << "set contents (inorder): ";
    s.inorder(); /// 应输出 1 2 3 4

    std::cout << "count(3): " << s.count(3) << '\n';
    std::cout << "count(5): " << s.count(5) << '\n';

    std::cout << "find(2): ";
    auto it = s.find(2);
    if (it != s.end()) std::cout << *it << '\n';

    std::cout << "set size: " << s.size() << '\n';
}

