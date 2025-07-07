/**
 * @file      my_ratio_test.cpp
 * @brief     [ratio_test]
 * @author    Weijh
 * @version   1.0
 */

#include "../chrono_/my_ratio.h"
#include <iostream>

using namespace my;

// 简化用别名
using r1 = ratio<4, 6>;   // 2/3
using r2 = ratio<-2, 4>;  // -1/2

// 加法
using r_add = ratio_add<r1, r2>;  // 2/3 + (-1/2) = 1/6
static_assert(r_add::num == 1 && r_add::den == 6, "Add failed");

// 减法
using r_sub = ratio_subtract<r1, r2>;  // 2/3 - (-1/2) = 7/6
static_assert(r_sub::num == 7 && r_sub::den == 6, "Sub failed");

// 乘法
using r_mul = ratio_multiply<r1, r2>;  // 2/3 * (-1/2) = -1/3
static_assert(r_mul::num == -1 && r_mul::den == 3, "Mul failed");

// 除法
using r_div = ratio_divide<r1, r2>;    // (2/3) / (-1/2) = -4/3
static_assert(r_div::num == -4 && r_div::den == 3, "Div failed");

// 判断相等
using r3 = ratio<10, 15>; // 2/3
static_assert(ratio_equal<r1, r3>::value, "Equal failed");

// 不等
static_assert(ratio_not_equal<r1, r2>::value, "Not Equal failed");

// ratio_less
static_assert(ratio_less<r2, r1>::value, "Less failed");

// 测试 gcd 与 lcm
static_assert(static_gcd_<36, 60>::value == 12, "GCD failed");
static_assert(static_lcm_<6, 8>::value == 24, "LCM failed");

int main() {
    std::cout << "r1 = " << r1::num << "/" << r1::den << "\n";
    std::cout << "r2 = " << r2::num << "/" << r2::den << "\n";

    std::cout << "r_add = " << r_add::num << "/" << r_add::den << "\n";
    std::cout << "r_sub = " << r_sub::num << "/" << r_sub::den << "\n";
    std::cout << "r_mul = " << r_mul::num << "/" << r_mul::den << "\n";
    std::cout << "r_div = " << r_div::num << "/" << r_div::den << "\n";

    std::cout << "r3 = " << r3::num << "/" << r3::den << "\n";

    std::cout << std::boolalpha;
    std::cout << "r1 == r3 ? " << ratio_equal<r1, r3>::value << "\n";
    std::cout << "r1 != r2 ? " << ratio_not_equal<r1, r2>::value << "\n";
    std::cout << "r2 < r1 ? " << ratio_less<r2, r1>::value << "\n";

    std::cout << "gcd(36, 60) = " << static_gcd_<36, 60>::value << "\n";
    std::cout << "lcm(6, 8) = " << static_lcm_<6, 8>::value << "\n";
    /**
            r1 = 2/3
            r2 = -1/2
            r_add = 1/6
            r_sub = 7/6
            r_mul = -1/3
            r_div = -4/3
            r3 = 2/3
            r1 == r3 ? true
            r1 != r2 ? true
            r2 < r1 ? true
            gcd(36, 60) = 12
            lcm(6, 8) = 24
     * */

    return 0;
}

