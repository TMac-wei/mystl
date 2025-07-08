/**
 * @file      my_priority_queue_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_priority_queue.h"
#include <iostream>
#include "../my_string.h"

void test_basic()
{
    my::priority_queue<int> pq;
    pq.push(10);
    pq.push(5);
    pq.push(20);

    std::cout << "Top should be 20: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Top should be 10: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Top should be 5: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Is empty? " << std::boolalpha << pq.empty() << std::endl;
    /**
        Top should be 20: 20
        Top should be 10: 10
        Top should be 5: 5
        Is empty? true
 * */
}

void test_emplace()
{
    my::priority_queue<std::string> pq;
    pq.emplace(5, 'a');  // creates "aaaaa"
    pq.emplace("hello");
    pq.emplace("zebra");

    std::cout << "Top string should be zebra: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Top string should be hello: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Top string should be aaaaa: " << pq.top() << std::endl;
    pq.pop();
    std::cout << "Is empty? " << std::boolalpha << pq.empty() << std::endl;
    /**
         Top string should be zebra: zebra
         Top string should be hello: hello
         Top string should be aaaaa: aaaaa
         Is empty? true
      * */
}

void test_custom_compare()
{
    /// 小根堆的建立
    my::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    pq.push(10);
    pq.push(5);
    pq.push(20);

    std::cout << "Top should be 5 (min heap): " << pq.top() << std::endl;
    /// Top should be 5 (min heap): 5
}

void test_range_constructor()
{
    /// 范围构造--大根堆
    std::vector<int> vec = {1, 3, 5, 2, 4};
    my::priority_queue<int> pq(vec.begin(), vec.end());
    std::cout << "Top should be 5: " << pq.top() << std::endl;
    /// Top should be 5: 5

}

int main()
{
    test_basic();
    test_emplace();
    test_custom_compare();
    test_range_constructor();
    return 0;
}
