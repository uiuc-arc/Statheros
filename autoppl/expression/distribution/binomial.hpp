#pragma once
#include <cassert>
#include <random>
#include <fastad>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/expression/distribution/uniform.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename p_type, typename n_type, typename value_t_=util::disc_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
template <util::var_expr p_type, util::var_expr n_type> // NOT CHANGED
#endif


struct Binomial : util::DistExpr<Binomial<p_type, n_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<p_type>);
    static_assert(util::assert_is_var_expr_v<n_type>);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<DiscreteUniform<p_type, n_type>, dist_value_t_>; 
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;
    // using fix_base_t = typename value_t::base_type;

    DiscreteUniform(p_type p, n_type n)
        : p_{p}, n_{n} 
    {}

    //integers
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_integral_v<value_t_dummy>>* = nullptr) const
    {
        std::binomial_distribution dist(n_, p_); 
        return dist(gen);
    }

    //fixed points
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_point_v<value_t_dummy>>* = nullptr) const
    {
	//auto dist = Uniform<p_type,n_type,value_t,dist_value_t>(min(), max()+one_);
	//return math::floor(dist.sample(gen));

    }

    //if we are using an integer as the value_t
    template <class value_t_dummy = value_t>
    dist_value_t pdf(value_t x, size_t index=0, std::enable_if_t<std::is_integral_v<value_t_dummy>>* = nullptr) const
    {	//is_integral already insures x is an int
        return (min(index) <= x && x <= max(index) ) ? one_d / dist_value_t(max(index) - min(index) + one_) : zero_;
    }

    template <class value_t_dummy = value_t>
    dist_value_t pdf(value_t x, size_t index=0,  std::enable_if_t<ppl::is_fixed_point_v<value_t_dummy>>* = nullptr) const
    {
        return (min(index) <= x && x <= max(index) && math::is_int(x)) ? one_d / dist_value_t(max(index) - min(index) + one_) : zero_;
    }


    dist_value_t log_pdf(value_t x, size_t index=0) const
    {
        using namespace math;
    	//is_integral already insures x is an int
        return (min(index) <= x && x <= max(index) && math::is_int(x)) ? 
            -log(max(index) - min(index) + one_) : 
            std::numeric_limits<dist_value_t>::lowest();
    }





    /**
     *  NOT MODIFIED YET FOR FIXED POINT
     */
    template <class ADVarType, class VecRefType, class VecADVarType>
    auto ad_log_pdf(const ADVarType& x,
                    const VecRefType& keys,
                    const VecADVarType& vars,
                    size_t idx = 0) const
    {
        auto&& ad_min_expr = min_.get_ad(keys, vars, idx);
        auto&& ad_max_expr = max_.get_ad(keys, vars, idx);
        return ad::if_else(
                ((ad_min_expr < x) && (x < ad_max_expr)),
                -ad::log(ad_max_expr - ad_min_expr),
                ad::constant(std::numeric_limits<dist_value_t>::lowest())
                );
    }

    value_t min(size_t index=0) const { return min_.get_value(index); }
    value_t max(size_t index=0) const { return max_.get_value(index); }

private:
    p_type p_;
    n_type n_;
    value_t one_ = value_t(1);
    value_t zero_ = value_t(0);
    dist_value_t one_d = dist_value_t(1);

};

} // namespace expr
} // namespace ppl
