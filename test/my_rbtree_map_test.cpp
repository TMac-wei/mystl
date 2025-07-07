/**
 * @file      my_rbtree_map_test.cpp
 * @brief     [my_rbtree_map_test]
 * @author    Weijh
 * @version   1.0
 */

#include <iostream>
#include "my_rbtree_map.h"

#include <string>

void test_map_like_rb_tree() {
    std::cout << "==== 测试 InsertUnique（模拟 map） ====\n";

    my::RBTree<int, std::string> map_like_tree;

    map_like_tree.InsertUnique({3, "three"});
    map_like_tree.InsertUnique({1, "one"});
    map_like_tree.InsertUnique({4, "four"});
    map_like_tree.InsertUnique({2, "two"});

    // 尝试插入重复键
    auto [it_dup, inserted] = map_like_tree.InsertUnique({2, "two-again"});
    std::cout << "尝试插入重复键 2，成功？ " << (inserted ? "Yes" : "No") << "\n";

    std::cout << "中序遍历输出 (key:value)：\n";
    map_like_tree.Inorder();

    std::cout << "Size: " << map_like_tree.Size() << "\n";

    std::cout << "Find key = 3: ";
    auto it = map_like_tree.Find(3);
    if (it != map_like_tree.end()) {
        std::cout << it->first << ":" << it->second << "\n";
    } else {
        std::cout << "not found\n";
    }

    std::cout << "Count key = 2: " << map_like_tree.Count(2) << "\n";
    std::cout << "Count key = 99: " << map_like_tree.Count(99) << "\n";

    std::cout << "遍历所有键值对:\n";
    for (auto iter = map_like_tree.begin(); iter != map_like_tree.end(); ++iter) {
        std::cout << iter->first << " -> " << iter->second << "\n";
    }
}

void test_multimap_like_rb_tree() {
    std::cout << "\n==== 测试 InsertMulti（模拟 multimap） ====\n";

    my::RBTree<int, std::string> multimap_like_tree;

    multimap_like_tree.InsertMulti({5, "apple"});
    multimap_like_tree.InsertMulti({2, "banana"});
    multimap_like_tree.InsertMulti({5, "apricot"});
    multimap_like_tree.InsertMulti({2, "blueberry"});
    multimap_like_tree.InsertMulti({8, "cherry"});

    std::cout << "中序遍历输出 (key:value)：\n";
    multimap_like_tree.Inorder();

    std::cout << "Count key = 2: " << multimap_like_tree.Count(2) << "\n";
    std::cout << "Count key = 5: " << multimap_like_tree.Count(5) << "\n";

    std::cout << "Size: " << multimap_like_tree.Size() << "\n";

    std::cout << "遍历所有键值对:\n";
    for (auto iter = multimap_like_tree.begin(); iter != multimap_like_tree.end(); ++iter) {
        std::cout << iter->first << " -> " << iter->second << "\n";
    }
}

int main() {
    system("chcp 65001");
    test_map_like_rb_tree();
    test_multimap_like_rb_tree();
    return 0;
}



