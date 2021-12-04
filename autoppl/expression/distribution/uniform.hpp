#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/math/math.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename min_type, typename max_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
template <util::var_expr min_type, util::var_expr max_type> 
#endif

struct Uniform : util::DistExpr<Uniform<min_type, max_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<min_type>);
    static_assert(util::assert_is_var_expr_v<max_type>);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Uniform<min_type, max_type>, dist_value_t_>; 
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;

    Uniform(min_type min, max_type max)
        : min_{min}, max_{max}, lowest_dist_value_{std::numeric_limits<dist_value_t>::lowest()}
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, min_type> && std::is_base_of_v<expr::ConstantBaseType, max_type>) {
            log_pdf_value_ = -math::log(static_cast<dist_value_t>(max.get_value()) - static_cast<dist_value_t>(min.get_value()));
        }
    }

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
        std::uniform_real_distribution dist(min(), max()); 
        return dist(gen);
    }

    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
        auto sign_and_int_bits = value_t::integer_bits;

        uint32_t random32 = gen();
        random32 = random32 << sign_and_int_bits;
        random32 = random32 >> sign_and_int_bits;
        value_t uniform_0_1_sample = value_t::from_raw_value(random32);

        return uniform_0_1_sample * (max(index) - min(index)) + min(index);
    }

    dist_value_t pdf(value_t x, size_t index=0) const
    {
        return (min(index) < x && x < max(index)) ? one_ / (max(index) - min(index)) : zero_;
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const
    {
        if (min(index) < x && x < max(index)) {
            if constexpr (std::is_base_of_v<expr::ConstantBaseType, min_type> && std::is_base_of_v<expr::ConstantBaseType, max_type>) {
                return log_pdf_value_;
            } else {
                return -math::log(static_cast<dist_value_t>(max(index)) - static_cast<dist_value_t>(min(index)));
            }
        }
        return lowest_dist_value_;
    }

    value_t min(size_t index=0) const { return min_.get_value(index); }
    value_t max(size_t index=0) const { return max_.get_value(index); }


    std::set<boost::uuids::uuid> getDeps() {
        std::set<boost::uuids::uuid> used_vars; 
        std::set<boost::uuids::uuid> min_used_vars = min_.getDeps(); 
        used_vars.insert(min_used_vars.begin(),min_used_vars.end());
        std::set<boost::uuids::uuid> max_used_vars = max_.getDeps();
        used_vars.insert(max_used_vars.begin(),max_used_vars.end());
        return used_vars;
    }

    DeterministicInterval getInterval() {
        DeterministicInterval min_interval = min_.getInterval();
        DeterministicInterval max_interval = max_.getInterval();
        return DeterministicInterval(min_interval.lower,max_interval.upper);
    }


    DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval) {
        DeterministicInterval min_interval = min_.getInterval();
        biggestInterval = union_(biggestInterval,min_interval);

        DeterministicInterval max_interval = max_.getInterval();
        biggestInterval = union_(biggestInterval,max_interval);

        DeterministicInterval LikelihoodInterval = sub(max_interval,min_interval);
        biggestInterval = union_(biggestInterval,dataInterval);

        return LikelihoodInterval;
    }

private:
    min_type min_;
    max_type max_;
    dist_value_t one_ = dist_value_t(1);
    dist_value_t zero_ = dist_value_t(0);

    // precomputed log_pdf when max and min are both constant
    dist_value_t log_pdf_value_;
    
    dist_value_t lowest_dist_value_;
};


//simpler primitive for when one doesn't need the entire class (i.e. one doesn't need to do the analysis)
//the sampling code is exactly the same...
template<class T=util::cont_param_t,class GenT>
T standard_uniform_primitive(GenT& gen) {
	if constexpr (ppl::is_fixed_point_v<T>) {
		auto sign_and_int_bits = T::integer_bits;
		uint32_t random32 = gen();
		random32 = random32 << sign_and_int_bits;
		random32 = random32 >> sign_and_int_bits;
		T uniform_0_1_sample = T::from_raw_value(random32);
		return uniform_0_1_sample;
	} else {
        std::uniform_real_distribution dist(0., 1.); 
        return dist(gen);
	}
}

} // namespace expr
} // namespace ppl
