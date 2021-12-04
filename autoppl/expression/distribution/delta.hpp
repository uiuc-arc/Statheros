#pragma once
#include <cassert>
#include <random>
#include <type_traits>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/math/math.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename p_type, typename value_t_=util::value_t_bernoulli, typename dist_value_t_=util::dist_value_t_bernoulli>
#else
template <util::var_expr p_type> // NOT CHANGED
#endif


struct Delta : util::DistExpr<Delta<p_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<p_type>);
#endif

    using value_t = value_t_;
    using param_value_t = typename util::var_expr_traits<p_type>::value_t;
    using base_t = util::DistExpr<Delta<p_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;

    Delta(p_type p)
        : p_{p}, lowest_dist_value_{std::numeric_limits<dist_value_t>::lowest()}
    {}

    //template var value_t is an int or long or something
    template <class GeneratorType, class value_t_dummy = value_t>								    
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_integral_v<value_t_dummy>>* = nullptr) const
    {
	    return value_t(p());
    }

    //template var is some fixed point instantiation (equiv. value_t = fixed<sometype,someoverflowtype,someint>)
    template <class GeneratorType, class value_t_dummy = value_t>								
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_point_v<value_t_dummy>>* = nullptr) const
    {
	    return value_t(p());
    }

    dist_value_t pdf(value_t x, size_t index=0) const
    {
        if (x == value_t(p()) ) return one_;
        else return zero_;
    }

    //Generic version (for any kind of int/fixed point)
    dist_value_t log_pdf(value_t x, size_t index=0) const
    {
	    if (x == value_t(p()) ) return zero_;
        else { return lowest_dist_value_; }
    }

    param_value_t p(size_t index=0) const 
    {
        return std::max(
                std::min(
                    p_.get_value(index), 
                    static_cast<param_value_t>(max())
                ), 
                static_cast<param_value_t>(min()) 
            ); 
    }

    // need to add min() and max()
    value_t min() const { return p_.get_value(); }
    value_t max() const { return p_.get_value(); }

private:
    p_type p_;
    dist_value_t one_ = value_t(1);
    dist_value_t zero_ = value_t(0);

    dist_value_t lowest_dist_value_;
};

} // namespace expr
} // namespace ppl
