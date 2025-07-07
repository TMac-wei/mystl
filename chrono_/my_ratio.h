/**
 * @file      my_ratio.h
 * @brief     [复现chrono中的ratio，之后以此为基础完成duration等]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include <cstdint>
#include <limits>
#include <climits>

/// 以下是关于在时间序列中的ratio

namespace my {
    /// 绝对值  intmax_t-->long long
    template<intmax_t Xp_>
    struct static_abs_ {
        static constexpr intmax_t value = Xp_ < 0 ? -Xp_ : Xp_;
    };

    /// 正负号
    template<intmax_t Xp_>
    struct static_sign_ {
        /// 如果传进来的Xp == 0则没有符号，都是0，否则判断正负号
        static constexpr intmax_t value = Xp_ == 0 ? 0 : (Xp_ < 0 ? -1 : 1);
    };

    /// 求最大公约数，辗转相除法  假设 Xp_ >= Yp_
    template<intmax_t Xp_, intmax_t Yp_>
    struct static_gcd_ {
        static constexpr intmax_t value = static_gcd_<Yp_, Xp_ % Yp_>::value;
    };

    /// 提供特化版本
    template<intmax_t Xp_>
    struct static_gcd_<Xp_, 0> {
        static constexpr intmax_t value = Xp_;
    };

    /// gdc(0, 0) = 1
    template<>
    struct static_gcd_<0, 0> {
        static constexpr intmax_t value = 1;
    };

    /// 最小公倍数  Xp_ Yp_ 的最小公倍数的公式是： Xp_ * Yp_ / gdc(Xp_, Yp_)
    template<intmax_t Xp_, intmax_t Yp_>
    struct static_lcm_ {
        static constexpr intmax_t value = Xp_ / static_gcd_<Xp_, Yp_>::value * Yp_;
    };

    /// 编译期安全地进行整数加减运算，并检查是否溢出，用于元编程中计算 intmax_t 类型的加法或减法
    template<intmax_t Xp_, intmax_t Yp_, intmax_t  = static_sign_<Yp_>::value>
    struct ll_add_;

    /// 分别给出2个特化版本，一个是 static_sign_<Yp_>::value> = 1， 一个是 static_sign_<Yp_>::value> = -1
    template<intmax_t Xp_, intmax_t Yp_>
    struct ll_add_<Xp_, Yp_, 1> {
    private:
        /// 需要判断是否溢出  c++中负数的最大值并不是对称的，所以用numeric_limits方式更安全
        static constexpr intmax_t min = std::numeric_limits<intmax_t>::min();
        static constexpr intmax_t max = std::numeric_limits<intmax_t>::max();
        static_assert(Xp_ <= max - Yp_, "overflow in ll_add_ addition");
    public:
        static constexpr intmax_t value = Xp_ + Yp_;
    };

    template<intmax_t Xp_, intmax_t Yp_>
    struct ll_add_<Xp_, Yp_, -1> {
    private:
        /// 需要判断是否溢出
        static constexpr intmax_t min = std::numeric_limits<intmax_t>::min();
        static constexpr intmax_t max = std::numeric_limits<intmax_t>::max();
        static_assert(min + Yp_ <= Xp_, "overflow in ll_add_ subtraction");
    public:
        static constexpr intmax_t value = Xp_ - Yp_;
    };

    template<intmax_t Xp_, intmax_t Yp_>
    struct ll_add_<Xp_, Yp_, 0> {
        static constexpr intmax_t value = Xp_;
    };

    /// *
    template<intmax_t Xp_, intmax_t Yp_>
    struct ll_mul_ {
        static constexpr intmax_t value = Xp_ * Yp_;
    };

    /// -
    template<intmax_t Xp_, intmax_t Yp_>
    struct ll_div_ {
        /// 除法要判断被除数是否为0
        static_assert(Yp_ != 0, "division by zero");
        static constexpr intmax_t value = Xp_ / Yp_;
    };


    /// 封装一个安全不会溢出的相加相减
    template<intmax_t Xp_, intmax_t Yp_>
    struct safe_add {
        static constexpr intmax_t value = ll_add_<Xp_, Yp_, 1>::value;
    };

    template<intmax_t Xp_, intmax_t Yp_>
    struct safe_sub {
        static constexpr intmax_t value = ll_add_<Xp_, Yp_, -1>::value;
    };

    template<intmax_t Xp_, intmax_t Yp_>
    struct safe_mul {
        static constexpr intmax_t value = ll_mul_<Xp_, Yp_>::value;
    };

    template<intmax_t Xp_, intmax_t Yp_>
    struct safe_div {
        static constexpr intmax_t value = ll_div_<Xp_, Yp_>::value;
    };


    /// ratio
    template<intmax_t Num_, intmax_t Den_ = 1>
    struct ratio {
    private:
        /// 分数分母不能为o
        static_assert(Den_ != 0, "denominator must not be zero");
        /// 然后分别得到分子分母的绝对值，最大公约数（分数化简）以及符号
        static constexpr intmax_t na_ = static_abs_<Num_>::value;
        static constexpr intmax_t de_ = static_abs_<Den_>::value;
        static constexpr intmax_t gcd_ = static_gcd_<na_, de_>::value;
        /// 最终的符号由分子分母的符号共同决定
        static constexpr intmax_t sig_ = static_sign_<Num_>::value * static_sign_<Den_>::value;

    public:
        /// 给出分数最简的分子与分母
        static constexpr intmax_t num = sig_ * (na_ / gcd_);
        static constexpr intmax_t den = de_ / gcd_;
        using type = ratio<num, den>;
    };

    template<typename R1, typename R2>
    struct ratio_less {
        /// 判断分数大小的方式，采用对角相乘
        static const bool value = R1::num * R2::den < R2::num * R1::den;
    };

    template<typename R1, typename R2>
    struct ratio_mul_ {
    private:
        static const intmax_t gcd_n1_d2_ = static_gcd_<R1::num, R2::den>::value;
        static const intmax_t gcd_d1_n2_ = static_gcd_<R1::den, R2::num>::value;

    public:
        using type = typename ratio<safe_mul<R1::num / gcd_n1_d2_, R2::num / gcd_d1_n2_>::value,
                safe_mul<R2::den / gcd_n1_d2_, R1::den / gcd_d1_n2_>::value>::type;
    };

    template<typename R1, typename R2>
    struct ratio_multiply : ratio_mul_<R1, R2>::type {
    };

    /// 除法
    template<typename R1, typename R2>
    struct ratio_div_ {
        static_assert(R2::num != 0, "Cannot divide by a ratio with zero numerator.");
        static_assert(R2::den != 0, "Cannot divide by a ratio with zero denominator.");

        using type = typename ratio<
                safe_mul<R1::num, R2::den>::value,
                safe_mul<R1::den, R2::num>::value
        >::type;
    };


    template<typename R1, typename R2>
    struct ratio_divide : ratio_div_<R1, R2>::type {
    };


    template<typename R1, typename R2>
    struct ratio_add_ {
    private:
        static const intmax_t gcd_n1_n2_ = static_gcd_<R1::num, R2::num>::value;
        static const intmax_t gcd_d1_d2_ = static_gcd_<R1::den, R2::den>::value;

        static constexpr intmax_t a = R1::num / gcd_n1_n2_;
        static constexpr intmax_t b = R2::num / gcd_n1_n2_;
        static constexpr intmax_t c = R2::den / gcd_d1_d2_;
        static constexpr intmax_t d = R1::den / gcd_d1_d2_;

        ///   a/d + b/c = （ac+bd)/dc
        /// 搞定分子
        static constexpr intmax_t numerator = safe_add<safe_mul<a, c>::value, safe_mul<b, d>::value>::value;
        /// 分母，分母的最小公倍数
        static constexpr intmax_t denominator = (R1::den / gcd_d1_d2_) * R2::den;

        /// 由 ratio_multiply 归约为最简分数
        using result_ratio = typename ratio_mul_<
                ratio<gcd_n1_n2_, 1>,
                ratio<numerator, denominator>
        >::type;

    public:
        using type = result_ratio;
    };

    template<class R1, class R2>
    struct ratio_gcd_ {
        typedef ratio<static_gcd_<R1::num, R2::num>::value, static_lcm_<R1::den, R2::den>::value> type;
    };

    template<typename R1, typename R2>
    using ratio_add = typename ratio_add_<R1, R2>::type;

    template<class R1, class R2>
    struct ratio_sub_ {
    private:
        static const intmax_t gcd_n1_n2_ = static_gcd_<R1::num, R2::num>::value;
        static const intmax_t gcd_d1_d2_ = static_gcd_<R1::den, R2::den>::value;

        static constexpr intmax_t a = R1::num / gcd_n1_n2_;
        static constexpr intmax_t b = R2::num / gcd_n1_n2_;
        static constexpr intmax_t c = R2::den / gcd_d1_d2_;
        static constexpr intmax_t d = R1::den / gcd_d1_d2_;

        ///   a/d - b/c = （ac-bd)/dc
        /// 搞定分子
        static constexpr intmax_t numerator = safe_sub<
                safe_mul<a, c>::value,
                safe_mul<b, d>::value
        >::value;

        /// 分母，分母的最小公倍数
        static constexpr intmax_t denominator = (R1::den / gcd_d1_d2_) * R2::den;

        /// 由 ratio_multiply 归约为最简分数
        using result_ratio = typename ratio_mul_<
                ratio<gcd_n1_n2_, 1>,
                ratio<numerator, denominator>
        >::type;


    public:
        using type = result_ratio;
    };

    template<class R1, class R2>
    using ratio_subtract = typename ratio_sub_<R1, R2>::type;

// ratio_equal
    template<class R1, class R2>
    struct ratio_equal {
        static const bool value = R1::num == R2::num && R1::den == R2::den;
    };

// ratio_not_equal
    template<class R1, class R2>
    struct ratio_not_equal {
        static const bool value = !ratio_equal<R1, R2>::value;
    };
}
