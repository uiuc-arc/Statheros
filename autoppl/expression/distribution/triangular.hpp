#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>
#include <autoppl/expression/distribution/uniform.hpp>


namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename a_type, typename b_type, typename c_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
template <util::var_expr a_type, util::var_expr b_type, util::var_expr c_type>
#endif


struct Triangular : util::DistExpr<Triangular<a_type, b_type, c_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<a_type>);
    static_assert(util::assert_is_var_expr_v<b_type>);
    static_assert(util::assert_is_var_expr_v<c_type>);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Triangular<a_type, b_type, c_type>, dist_value_t_>; 
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;

    Triangular(a_type a, b_type b, c_type c)
        : a_{a}, b_{b}, c_{c}, lowest_dist_value_{std::numeric_limits<dist_value_t>::lowest()}, log_two_{math::log(two_)}
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, a_type> 
                && std::is_base_of_v<expr::ConstantBaseType, b_type>
                && std::is_base_of_v<expr::ConstantBaseType, c_type>) {
            a_dist_value_ = static_cast<dist_value_t>(a.get_value());
            b_dist_value_ = static_cast<dist_value_t>(b.get_value());
            c_dist_value_ = static_cast<dist_value_t>(c.get_value());
            log_bv_av_ = math::log(b_dist_value_ - a_dist_value_);
            log_cv_av_ = math::log(c_dist_value_ - a_dist_value_);
            log_bv_cv_ = math::log(b_dist_value_ - c_dist_value_);
            C_constant_ = log_two_ - log_bv_av_ - log_cv_av_;
            B_constant_ = log_two_ - log_bv_av_ - log_bv_cv_;
        }
    }

    //same version for both
    template <class GeneratorType>
    value_t sample(GeneratorType& gen) const
    {
        auto a_v = a_.get_value(index);
        auto b_v = b_.get_value(index);
        auto c_v = c_.get_value(index);

        value_t U = standard_uniform_primitive<value_t>(gen);
        if ( U < ((c_v-a_v)/(b_v-a_v)) ){
            return a_v + ppl::math::sqrt(U*(c_v-a_v)*(b_v-a_v));
        }
        else {
            return b_v - ppl::math::sqrt( (one_-U)*(b_v-c_v)*(b_v-a_v) );
        }
    }

    dist_value_t pdf(value_t x, size_t index=0) const
    {
        auto a_v = a_.get_value(index);
        auto b_v = b_.get_value(index);
        auto c_v = c_.get_value(index);

        if (x < a_v){
            return zero_;
        } else if (x <= c_v){
            return  ((two_ /(b_v - a_v)) * ((x-a_v)/(c_v-a_v)));
        } else if (x <= b_v) {
            return  ((two_ /(b_v - a_v)) * ((b_v-x)/(b_v-c_v)));
        } else {
            return zero_;
        }
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const
    {
        using namespace math;
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, a_type> 
                && std::is_base_of_v<expr::ConstantBaseType, b_type>
                && std::is_base_of_v<expr::ConstantBaseType, c_type>) {
            dist_value_t x_dist = static_cast<dist_value_t>(x);

            if (x_dist < a_dist_value_){
                return lowest_dist_value_;
            } else if (x_dist <= c_dist_value_){
                return C_constant_ + log(x_dist - a_dist_value_);
            } else if (x_dist <= b_dist_value_) {
                return B_constant_ + log(b_dist_value_ - x_dist);
            } else {
                return lowest_dist_value_;
            }
        } else {
            auto a_v = static_cast<dist_value_t>(a_.get_value(index));
            auto b_v = static_cast<dist_value_t>(b_.get_value(index));
            auto c_v = static_cast<dist_value_t>(c_.get_value(index));

            dist_value_t x_dist = static_cast<dist_value_t>(x);

            if (x_dist < a_v){
                return lowest_dist_value_;
            } else if (x_dist <= c_v){
                return log_two_ - log(b_v - a_v) + log(x_dist - a_v) - log(c_v - a_v);
            } else if (x_dist <= b_v) {
                return log_two_ - log(b_v - a_v) + log(b_v - x_dist) - log(b_v - c_v);
            } else {
                return lowest_dist_value_;
            }
        }
    }

    value_t min(size_t index=0) const { return a_.get_value(index); }
    value_t max(size_t index=0) const { return b_.get_value(index); }


    std::set<boost::uuids::uuid> getDeps(){
        std::set<boost::uuids::uuid> used_vars; 
        std::set<boost::uuids::uuid> min_used_vars = a_.getDeps();  
        used_vars.insert(min_used_vars.begin(),min_used_vars.end());
        std::set<boost::uuids::uuid> max_used_vars = b_.getDeps();
        used_vars.insert(max_used_vars.begin(),max_used_vars.end());
        return used_vars;
    }

   DeterministicInterval getInterval(){
        DeterministicInterval min_interval = a_.getInterval();
        DeterministicInterval max_interval = b_.getInterval();
        return DeterministicInterval(min_interval.lower,max_interval.upper);
   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){

	return dataInterval;
   }



private:
    a_type a_;
    b_type b_;
    c_type c_;

    dist_value_t two_ = dist_value_t(2);
    dist_value_t one_ = dist_value_t(1);
    dist_value_t zero_ = dist_value_t(0);

    dist_value_t lowest_dist_value_;
    dist_value_t log_two_;

    // precomputed constants when a, b, or c is constant
    dist_value_t a_dist_value_, b_dist_value_, c_dist_value_;
    dist_value_t log_bv_av_, log_cv_av_, log_bv_cv_;
    dist_value_t C_constant_, B_constant_;
};




} // namespace expr
} // namespace ppl
