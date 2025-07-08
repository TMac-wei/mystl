/**
 * @file      my_duration_test.cpp
 * @brief     [my_duration_test]
 * @author    Weijh
 * @version   1.0
 */

#include "chrono_/my_duration.h"

using namespace my;


int main() {
    milliseconds ms(1500); // 1500ms
    seconds s(2);          // 2s = 2000ms

    std::cout << "ms.count() = " << ms.count() << "ms\n";
    std::cout << "seconds.count() = " << s.count() << "s\n";

    auto res = ms + s;     // 3500ms
    std::cout << "res.count() = " << res.count() << "ms\n";  // 应输出 3500

    auto diff = s - ms;    // 2000ms - 1500ms = 500ms
    std::cout << "diff.count() = " << diff.count() << "ms\n"; // 应输出 500


    std::cout << "Hello, world!" << std::endl;
    my::nanoseconds ns(10000000);
    std::cout << ns.count() << std::endl;
    my::microseconds us = my::duration_cast<my::microseconds>(ns);
    std::cout << us.count() << std::endl;
    my::milliseconds ms2 = my::duration_cast<my::milliseconds>(ns);
    std::cout << ms2.count() << std::endl;

    my::nanoseconds ns2(10000000);
    std::cout << ns2.count() << std::endl;

    auto n3 = ns + ns2;
    std::cout << n3.count() << std::endl;

    auto n4 = ns - ns2;
    std::cout << n4.count() << std::endl;

    auto n5 = ns + us + ms2;
    std::cout << n5.count() << std::endl;

    ms2 /= 2;
    std::cout << ms2.count() << std::endl;

    /**
            ms.count() = 1500ms
            seconds.count() = 2s
            res.count() = 3500ms
            diff.count() = 500ms
            Hello, world!
            10000000
            10000
            10
            10000000
            20000000
            0
            30000000
            5
     * */
}
