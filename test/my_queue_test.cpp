/**
 * @file      my_queue_test.cpp
 * @brief     [queue_test]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_queue.h"
#include <iostream>

int main()
{
    my::queue<int> que;
    for (int i = 1; i <= 5; i++) {
        que.push(i);
    }
    while (!que.empty()) {
        std::cout <<  "first element of queue is : " << que.front() << ", and  ";
        std::cout << "size of queue is : "<< que.size() << "\n ";
        que.pop();
    }
    /**
             first element of queue is : 1, and  size of queue is : 5
             first element of queue is : 2, and  size of queue is : 4
             first element of queue is : 3, and  size of queue is : 3
             first element of queue is : 4, and  size of queue is : 2
             first element of queue is : 5, and  size of queue is : 1
     * */
    // 1 2 3 4 5
}
