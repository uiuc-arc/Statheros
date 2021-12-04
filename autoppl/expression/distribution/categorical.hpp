#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>
#include <vector>

namespace ppl {
namespace expr {


#if __cplusplus <= 201703L
template <typename p_type, typename value_t_=util::disc_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
//template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct Categorical : util::DistExpr<Categorical<p_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L

#endif

    using value_t = value_t_;
    using param_value_t = typename util::var_expr_traits<p_type>::value_t;
    using base_t = util::DistExpr<Categorical<p_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    Categorical(p_type p1, p_type p2, p_type p3)
        : p1_{p1}, p2_{p2}, p3_{p3}, lowest_dist_value_{std::numeric_limits<dist_value_t>::lowest()}
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            log_p1_ = math::log(p1.get_value());
            log_p2_ = math::log(p2.get_value());
            log_p3_ = math::log(p3.get_value());
        }
    }

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
        value_t a = standard_uniform_primitive<value_t>(gen);
        if (a < p1()){
            return zero_;
        }
        else if (a < p1()+p2()){
            return one_;
        }
        else {
            return two_;
        }
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
        value_t a = standard_uniform_primitive<value_t>(gen);
        if (a < p1()){
            return zero_;
        }
        else if (a < p1()+p2()){
            return one_;
        }
        else {
            return two_;
        }
    }


    // for integral or fixed-point types
    template <class T, typename std::enable_if_t<std::is_integral_v<T> || ppl::is_fixed_point_v<T>, T>* = nullptr>
    dist_value_t pdf(T x, size_t index=0) const
    {
        if (x == one_){ return p2(index);}
        else if (x == zero_){ return p1(index);}
	    else if (x == two_){ return p3(index);}
        else return zero_;
    }

    // for floating-point types, avoids casting from a floating point to an int
    template <class T, typename std::enable_if_t<std::is_floating_point_v<T>, T>* = nullptr>
    dist_value_t pdf(T x, size_t index=0) const
    {
        if (x == one_){ return p2(index);}
        else if (x == zero_){ return p1(index);}
	    else if (x == two_){ return p3(index);}
        else return zero_;
    }


    // for integral or fixed-point types
    template <class T, typename std::enable_if_t<std::is_integral_v<T> || ppl::is_fixed_point_v<T>, T>* = nullptr>
    dist_value_t log_pdf(T x, size_t index=0) const
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            if (x == one_) { return log_p2_; }
            else if (x == zero_) { return log_p1_; }
            else if (x == two_) { return log_p3_; }
        } else {
            if (x == one_) { return math::log(p2(index)); }
            else if (x == zero_) { return math::log(p1(index)); }
            else if (x == two_) { return math::log(p3(index)); }
        }
        return lowest_dist_value_;
    }

    // for floating-point types, avoids casting from a floating point to an int
    template <class T, typename std::enable_if_t<std::is_floating_point_v<T>, T>* = nullptr>
    dist_value_t log_pdf(T x, size_t index=0) const
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, p_type>) {
            if (x == one_) { return log_p2_; }
            else if (x == zero_) { return log_p1_; }
            else if (x == two_) { return log_p3_; }
        } else {
            if (x == one_) { return math::log(p2(index)); }
            else if (x == zero_) { return math::log(p1(index)); }
            else if (x == two_) { return math::log(p3(index)); }
        }
        return lowest_dist_value_;
    }


    param_value_t p1(size_t index=0) const 
    {
        //using namespace math;
        return math::max(
            math::min(
                p1_.get_value(index), 
                static_cast<param_value_t>(max())
            ),
            static_cast<param_value_t>(min()) 
        );
    }


    param_value_t p2(size_t index=0) const 
    {
        //using namespace math;
        return math::max(
            math::min(
                p2_.get_value(index), 
                static_cast<param_value_t>(max())
            ),
            static_cast<param_value_t>(min()) 
        );
    }


    param_value_t p3(size_t index=0) const 
    {
        //using namespace math;
        return math::max(
            math::min(
                p3_.get_value(index), 
                static_cast<param_value_t>(max())
            ),
            static_cast<param_value_t>(min()) 
        );
    }


    value_t min() const { return zero_; }
    value_t max() const {return two_;}


    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p_type p1_;
    p_type p2_;
    p_type p3_;

    value_t zero_ = value_t(0);
    value_t one_ = value_t(1);
    value_t two_ = value_t(2);

    // precomputed log_pdf when p1, p2, or p3 is constant
    dist_value_t log_p1_;
    dist_value_t log_p2_;
    dist_value_t log_p3_;

    dist_value_t lowest_dist_value_;
};


} // namespace expr
} // namespace ppl
