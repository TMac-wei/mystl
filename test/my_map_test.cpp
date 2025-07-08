/**
 * @file      my_map_test.cpp
 * @brief     [测试map]
 * @author    Weijh
 * @version   1.0
 */

#include "my_map.h"
#include <iostream>
#include <cassert>
#include <string>

void test() {
    my::map<int, std::string> m;
//    m.insert({2, "two"});
    m[3] = "three";

// 修改值测试
//    m[2] = "TWO";
    m[2] = "two";
    m.insert({2, "t11wo"});
    assert(m[2] == "two");

//    assert(m[2] == "TWO");

// 遍历测试
    for (auto& kv : m) {
        std::cout << kv.first << ": " << kv.second << "\n";
    }

}

void test_my_map() {
    std::cout << ">>> Testing my::map...\n";

    my::map<int, std::string> m;

    // operator[]
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";

    assert(m.size() == 3);
    std::cout << "m[1]:" << m[1] << std::endl;
    std::cout << "m[2]:" << m[2] << std::endl;
    std::cout << "m[3]:" << m[3] << std::endl;
    assert(m[2] == "two");

    // insert duplicate key
    auto res = m.insert({2, "TWO"});
    assert(res.second == false); // insertion should fail
    assert(res.first->second == "two");

    // find
    auto it = m.find(3);
    assert(it != m.end());
    assert(it->second == "three");

    // range insert
    std::pair<const int, std::string> arr[] = {
            {4, "four"}, {5, "five"}, {6, "six"}
    };
    my::map<int, std::string> m2(arr, arr + 3);
    assert(m2.size() == 3);

    // initializer list
    my::map<int, std::string> m3 = {
            {10, "ten"}, {20, "twenty"}, {30, "thirty"}
    };
    assert(m3.size() == 3);
    assert(m3.find(20)->second == "twenty");

    std::cout << "Map in-order traversal:\n";
    m.inorder(); // should print in key order: 1,2,3

    std::cout << "my::map tests passed.\n\n";
}

void test_my_multimap() {
    std::cout << ">>> Testing my::multimap...\n";

    my::multimap<int, std::string> mm;

    mm.insert({1, "one"});
    mm.insert({2, "two"});
    mm.insert({2, "TWO"});
    mm.insert({3, "three"});
    assert(mm.size() == 4);

    // count
    assert(mm.count(2) == 2);
    assert(mm.count(3) == 1);
    assert(mm.count(4) == 0);

    // find
    auto it = mm.find(2);
    assert(it != mm.end());
    assert(it->first == 2);

    // range constructor
    std::pair<const int, std::string> arr[] = {
            {7, "seven"}, {8, "eight"}, {7, "SEVEN"}
    };
    my::multimap<int, std::string> mm2(arr, arr + 3);
    assert(mm2.count(7) == 2);

    // initializer list
    my::multimap<int, std::string> mm3 = {
            {100, "a"}, {100, "b"}, {200, "c"}
    };
    assert(mm3.count(100) == 2);
    assert(mm3.count(200) == 1);

    std::cout << "Multimap in-order traversal:\n";
    mm.inorder(); // should show 1:one 2:two 2:TWO 3:three

    std::cout << "my::multimap tests passed.\n";
}

int main() {
    test();
    test_my_map();
    test_my_multimap();
    return 0;
}




