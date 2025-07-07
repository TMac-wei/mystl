/**
 * @file      my_duration.h
 * @brief     [my_duration]
 * @author    Weijh
 * @date      2025/7/7
 * @version   1.0
 */

#pragma once

#include <iostream>
#include "my_ratio.h"

namespace my {

    template<class Rep_, class Period_ = ratio<1>>
    class duration;

    /// 用于将一个时间段从一个周期单位转换到另一个周期单位
    template<typename ToDuration_, typename Rep_, typename Period_>
    constexpr ToDuration_ duration_cast(const duration<Rep_, Period_> &d__) {
        using CF = ratio_divide<Period_, typename ToDuration_::period>;
        /// 类型转换（整型除法，可能发生截断）
        auto r_ = static_cast<typename ToDuration_::rep>(
                static_cast<long long>(d__.count()) * CF::num / CF::den
        );

        return ToDuration_(r_);
    }

    /// duration实现
    template<typename Rep_, typename Period_>
    class duration {
    public:
        using rep = Rep_;
        using period = Period_;

        constexpr duration()
                : rep_() {}

        template<typename Rep2_>
        explicit constexpr duration(const Rep2_ &r_)
                : rep_(r_) {}

        template<typename Rep2_, typename Period2_>
        constexpr duration(const duration<Rep2_, Period2_> &d_)
                : rep_(duration_cast<duration>(d_).count()) {}

        constexpr Rep_ count() const {
            return rep_;
        }


        /// operator+ / operator-：将两个不同周期单位的 duration 转换成共同单位后再相加/减
        constexpr duration operator+() const {
            return *this;
        }

        constexpr duration operator-() const {
            return duration(-count());
        }

        duration &operator++() {
            ++rep_;
            return *this;
        }

        duration operator++(int) {
            duration temp_(*this);
            ++*this;
            return temp_;
        }

        duration &operator--() {
            --rep_;
            return *this;
        }

        duration operator--(int) {
            duration temp_(*this);
            --*this;
            return temp_;
        }

        duration &operator+=(const duration &d_) {
            rep_ += d_.count();
            return *this;
        }

        duration &operator*=(const rep &rhs) {
            rep_ *= rhs;
            return *this;
        }

        duration &operator-=(const duration &d_) {
            rep_ -= d_.count();
            return *this;
        }

        duration &operator/=(const rep &rhs) {
            rep_ /= rhs;
            return *this;
        }

        duration &operator%=(const rep &rhs) {
            rep_ %= rhs;
            return *this;
        }

    private:
        Rep_ rep_;
    };

    /// 在两个 duration 类型中选取精度更高的单位
    template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
    struct higher_precision_duration {
        using type = typename std::conditional<
                ratio_less<Period1_, Period2_>::value,
                duration<Rep1_, Period1_>,
                duration<Rep2_, Period2_>>::type;

    };

    using nano = ratio<1, 1000000000>;
    using micro = ratio<1, 1000000>;
    using milli = ratio<1, 1000>;

    using nanoseconds = duration<int64_t, nano>;
    using microseconds = duration<int64_t, micro>;
    using milliseconds = duration<int64_t, milli>;
    using seconds = duration<int64_t, ratio<1>>;
    using minutes = duration<int64_t, ratio<60>>;
    using hours = duration<int64_t, ratio<3600>>;
    using days = duration<int64_t, ratio<86400>>;
}

/**
 * 找出两个 duration 类型中精度更高（周期更小）的类型；
 * 把两个 duration 都转换成该更高精度类型后再执行加减法；
 * 最终构造并返回该统一类型的结果。
 * */
template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
constexpr auto operator+(const my::duration<Rep1_, Period1_> &lhs,
                         const my::duration<Rep2_, Period2_> &rhs) {
    /// 找出这两个类型的最高精度的那一个
    /// 用 Dur_ 直接做目标类型，风险大（不一定匹配 lhs 和 rhs）
////    using Dur_ = typename my::higher_precision_duration<Rep1_, Period1_, Rep2_,Period2_>::type ;
///// rep具有差异，显式使用common_type
//    using CommonRep_  = typename std::common_type<typename Dur_::rep>::type;

    using PeriodCommon = typename my::higher_precision_duration<Rep1_, Period1_, Rep2_, Period2_>::type::period;

    /// 使用 common_type 处理 rep 类型差异
    using CommonRep = typename std::common_type<Rep1_, Rep2_>::type;
    using CommonDuration = my::duration<CommonRep, PeriodCommon>;
    /// rep具有差异，显式使用common_type
    /// 转换 lhs 和 rhs 到统一类型
    CommonDuration lhs_converted = my::duration_cast<CommonDuration>(lhs);
    CommonDuration rhs_converted = my::duration_cast<CommonDuration>(rhs);

    return CommonDuration(lhs_converted.count() + rhs_converted.count());
}

template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
constexpr auto operator-(const my::duration<Rep1_, Period1_> &lhs,
                         const my::duration<Rep2_, Period2_> &rhs) {
    /// 找出这两个类型的最高精度的那一个
//    using Dur_ = typename my::higher_precision_duration<Rep1_, Period1_, Rep2_,Period2_>::type ;
    /**
     * 这句只能确定周期更小的那个 duration<Rep, Period> 类型（例如 duration<int64_t, nano>），
     * 直接拿它当目标类型做 duration_cast<Dur_>(lhs) 是危险的，因为：lhs 和 rhs 的 rep 不一定等于 Dur_::rep；
     * ration_cast 内部使用 ratio_divide<Period_, ToDuration_::period>，如果 ToDuration_::period::den == 0，编译期会直接 static_assert 失败；
     * 所以不该直接把 Dur_ 当作目标类型使用*/
    /// rep具有差异，显式使用common_type
//    using CommonRep_  = typename std::common_type<typename Dur_::rep>::type;
//
//    CommonRep_ result = static_cast<CommonRep_>(my::duration_cast<Dur_>(lhs).count()) -
//                        static_cast<CommonRep_>(my::duration_cast<Dur_>(rhs).count());
//
//    return Dur_(result);

    /// 精度更高的周期单位
    using CommonPeriod = typename my::higher_precision_duration<Rep1_, Period1_, Rep2_, Period2_>::type::period;

    /// 更宽容的数值类型
    using CommonRep = typename std::common_type<Rep1_, Rep2_>::type;

    /// 构造统一类型
    using CommonDuration = my::duration<CommonRep, CommonPeriod>;

    /// 安全转换
    CommonRep lhs_val = my::duration_cast<CommonDuration>(lhs).count();
    CommonRep rhs_val = my::duration_cast<CommonDuration>(rhs).count();

    return CommonDuration(lhs_val - rhs_val);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator/(const my::duration<Rep1, Period1> &lhs,
                         const my::duration<Rep2, Period2> &rhs) {
    using CT = typename std::common_type<Rep1, Rep2>::type;
    return static_cast<CT>(lhs.count()) / static_cast<CT>(rhs.count());
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator%(const my::duration<Rep1, Period1> &lhs,
                         const my::duration<Rep2, Period2> &rhs) {
    using Common = typename std::common_type<my::duration<Rep1, Period1>, my::duration<Rep2, Period2>>::type;
    return Common(lhs).count() % Common(rhs).count();
}


