#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/expression/distribution/uniform.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename min_type, typename max_type, typename value_t_=util::disc_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
template <util::var_expr min_type, util::var_expr max_type> // NOT CHANGED
#endif


struct DiscreteUniform : util::DistExpr<DiscreteUniform<min_type, max_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<min_type>);
    static_assert(util::assert_is_var_expr_v<max_type>);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<DiscreteUniform<min_type, max_type>, dist_value_t_>; 
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;

    DiscreteUniform(min_type min, max_type max)
        : min_{min}, max_{max} 
    {}

    //integers
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_integral_v<value_t_dummy>>* = nullptr) const
    {
        std::uniform_int_distribution dist(min(), max()); 
        return dist(gen);
    }

    //fixed points
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_point_v<value_t_dummy>>* = nullptr) const
    {
        std::uniform_int_distribution dist(min().raw_value(), max().raw_value());
        return math::floor(value_t::from_raw_value(dist(gen)));
    }

    dist_value_t pdf(value_t x, size_t index=0) const
    {
        return (min(index) <= x && x <= max(index) && math::is_int(x)) ? one_ / (max(index) - min(index) + one_) : zero_;
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const
    {
        using namespace math;
        return (min(index) <= x && x <= max(index) && math::is_int(x)) ? 
            -log(static_cast<dist_value_t>(max(index) - min(index) + one_)) : 
            std::numeric_limits<dist_value_t>::lowest();
    }

    value_t min(size_t index=0) const { return min_.get_value(index); }
    value_t max(size_t index=0) const { return max_.get_value(index); }

private:
    min_type min_;
    max_type max_;
    dist_value_t one_ = dist_value_t(1);
    dist_value_t zero_ = dist_value_t(0);
};

} // namespace expr
} // namespace ppl
