/**
 * @file      my_tuple_test.cpp
 * @brief     [tuple_test]
 * @author    Weijh
 * @version   1.0
 */

#include "../my_tuple.h"

#include <iostream>

using namespace my;

int main()
{

    auto myTuple = make_tuple(1, 2.2, 'a');
    std::cout << "first element: " << get<0>(myTuple) << std::endl;
    std::cout << "second element: " << get<1>(myTuple) << std::endl;
    std::cout << "third element: " << get<2>(myTuple) << std::endl;

    auto myTuple2 = make_tuple(1, 2.2, "ddda");
    std::cout << "first element: " << get<0>(myTuple2) << std::endl;
    std::cout << "second element: " << get<1>(myTuple2) << std::endl;
    std::cout << "third element: " << std::string(get<2>(myTuple2)) << std::endl;
    /**
        first element: 1
        second element: 2.2
        third element: a
        first element: 1
        second element: 2.2
        third element: ddda

        进程已结束，退出代码为 0
     * */

    return 0;
}

