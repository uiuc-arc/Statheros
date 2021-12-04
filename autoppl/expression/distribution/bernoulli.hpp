#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename p_type, typename value_t_=util::value_t_bernoulli, typename dist_value_t_=util::dist_value_t_bernoulli>
#else
template <util::var_expr p_type> 
#endif


struct Bernoulli : util::DistExpr<Bernoulli<p_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<p_type>);
#endif

    using value_t = value_t_;
    using param_value_t = typename util::var_expr_traits<p_type>::value_t;
    using base_t = util::DistExpr<Bernoulli<p_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;

    Bernoulli(p_type p)
        : p_{p}, lowest_dist_value_{std::numeric_limits<dist_value_t>::lowest()}
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            log_pdf_one_ = math::log(p.get_value());
            log_pdf_zero_ = math::log(one_p - p.get_value());
        }
    }

    template <class GeneratorType, class value_t_dummy = value_t>								    
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_integral_v<value_t_dummy>>* = nullptr) const
    {
        std::bernoulli_distribution dist(p()); 
        return dist(gen);
    }

    template <class GeneratorType, class value_t_dummy = value_t>								
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_point_v<value_t_dummy>>* = nullptr) const
    {
        std::uniform_int_distribution dist(zero_.raw_value(), one_.raw_value());
        if (dist(gen) < p().raw_value()) {
            return one_;
        }
        return zero_;
    }

    // for integral or fixed-point types
    template <class T, typename std::enable_if_t<std::is_integral_v<T> || ppl::is_fixed_point_v<T>, T>* = nullptr>
    dist_value_t pdf(T x, size_t index=0) const
    {
        if (x == one_) return p(index);
        else if (x == zero_) return one_ - p();
        else return zero_;
    }

    // for floating-point types, avoids casting from a floating point to an int
    template <class T, typename std::enable_if_t<std::is_floating_point_v<T>, T>* = nullptr>
    dist_value_t pdf(T x, size_t index=0) const
    {
        if (x == one_) return p(index);
        else if (x == zero_) return one_ - p();
        else return zero_;
    }

    // for integral or fixed-point types
    template <class T, typename std::enable_if_t<std::is_integral_v<T> || ppl::is_fixed_point_v<T>, T>* = nullptr>
    dist_value_t log_pdf(T x, size_t index=0) const
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            if (x == one_) { return log_pdf_one_; }
            else if (x == zero_) { return log_pdf_zero_; }
        } else {
            if (x == one_) { return math::log(p(index)); }
            else if (x == zero_) { return math::log(one_p - p(index)); }
        }
        return lowest_dist_value_;
    }

    // for floating-point types, avoids casting from a floating point to an int
    template <class T, typename std::enable_if_t<std::is_floating_point_v<T>, T>* = nullptr>
    dist_value_t log_pdf(T x, size_t index=0) const
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            if (x == one_) { return log_pdf_zero_; }
            else if (x == zero_) { return log_pdf_one_; }
        } else {
            if (x == one_) { return math::log(p(index)); }
            else if (x == zero_) { return math::log(one_p - p(index)); }
        }
        return lowest_dist_value_;
    }

    param_value_t p(size_t index=0) const 
    {
        //using namespace math;
        return math::max(
            math::min(
                p_.get_value(index), 
                static_cast<param_value_t>(max())
            ),
            static_cast<param_value_t>(min()) 
        );
    }

    value_t min() const { return zero_; }
    value_t max() const { return one_; }

    DeterministicInterval getInterval() {
        return DeterministicInterval(0.0, 1.0);
    }

    DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval) {
        DeterministicInterval p_interval = p_.getInterval();
        biggestInterval = union_(biggestInterval,p_interval);

        DeterministicInterval complement_interval = sub(constant_(1.),p_interval);
        biggestInterval = union_(biggestInterval,complement_interval);

        DeterministicInterval log_p_interval = log_(p_interval);
        biggestInterval = union_(biggestInterval,log_p_interval);

        DeterministicInterval log_complement_interval = log_(complement_interval);
        biggestInterval = union_(biggestInterval,log_complement_interval);

        DeterministicInterval union_interval = union_(log_p_interval,log_complement_interval);
        biggestInterval = union_(biggestInterval,union_interval);

        return union_interval;
    }

private:
    p_type p_;

    param_value_t one_p = param_value_t(1);
    value_t one_ = value_t(1);
    value_t zero_ = value_t(0);

    // precompute log_pdf when p is constant
    dist_value_t log_pdf_one_;
    dist_value_t log_pdf_zero_;

    dist_value_t lowest_dist_value_;
};

} // namespace expr
} // namespace ppl
