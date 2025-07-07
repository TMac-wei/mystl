/**
 * @file      mt_pair_test.cpp
 * @brief     [pair_test]
 * @author    Weijh
 * @version   1.0
 */

#include <string>
#include <iostream>
#include "my_pair.h"
using namespace my;

int main() {
    pair<int, std::string> p1(1, std::string("hello"));
    pair<int, std::string> p2 = my::make_pair(2, std::string("world"));

    std::cout << "p1: (" << p1.first_ << ", " << p1.second_ << ")\n";
    std::cout << "p2: (" << p2.first_ << ", " << p2.second_ << ")\n";

    p1 = p2;
    std::cout << "After copy, p1: (" << p1.first_ << ", " << p1.second_ << ")\n";

    pair<int, std::string> p3 = std::move(p2);
    std::cout << "After move, p3: (" << p3.first_ << ", " << p3.second_ << ")\n";

    p1.swap(p3);
    std::cout << "After swap, p1: (" << p1.first_ << ", " << p1.second_ << ")\n";

    if (p1 != p3) {
        std::cout << "p1 and p3 are different.\n";
    }
    /**
        p1: (1, hello)
        p2: (2, world)
        After copy, p1: (2, world)
        After move, p3: (2, world)
        After swap, p1: (2, world)
     * */

    return 0;
}

