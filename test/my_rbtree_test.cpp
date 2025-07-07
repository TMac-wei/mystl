/**
 * @file      my_rbtree_test.cpp
 * @brief     [my_rbtree_test]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#include "my_rbtree.h"
#include <iostream>
#include <vector>

void TestInsertAndTraverse()
{
    RBTree<int> tree;
    std::vector<int> values = {10, 5, 20, 3, 7, 15, 30, 1, 6, 8, 31};

    std::cout << "[Insert] insert element:";
    for (int val : values)
    {
        tree.Insert(val);
        std::cout << val << " ";
    }
    std::cout << "\n";

    std::cout << "[Inorder Traverse] thr result of inorder:";
    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "[Reverse Traverse] reverse : ";
    auto it = tree.end();
    while (it != tree.begin())
    {
        --it;
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "[Validation]rbtree is ok or not? "
              << (tree.IsValidRBTree() ? "yes" : "no")
              << std::endl;
}

int main()
{
    TestInsertAndTraverse();
    /// [Insert] insert element:10 5 20 3 7 15 30 1 6 8
    /// [Inorder Traverse] thr result of inorder:1 3 5 6 7 8 10 15 20 30
    /// [Reverse Traverse] reverse : 30 20 15 10 8 7 6 5 3 1
    /// [Validation]rbtree is ok or not? yes
    ///
    /// 进程已结束，退出代码为 0
    return 0;
}
