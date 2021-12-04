#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/expression/variable/variable_viewer.hpp>
#include <autoppl/math/math.hpp>
#include <type_traits>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>

// MSVC does not seem to support M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace ppl::math;

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct Normal : util::DistExpr<Normal<mean_type, stddev_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_expr_v<mean_type>);
    static_assert(util::assert_is_var_expr_v<stddev_type>);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Normal<mean_type, stddev_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    Normal(mean_type mean, stddev_type stddev)
        : mean_{mean}, stddev_{stddev}
    {
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, mean_type>) {
            mu_dist_value_ = static_cast<dist_value_t>(mean.get_value());
        }
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, stddev_type>) {
            sigma_dist_value_ = static_cast<dist_value_t>(stddev.get_value());
            C_ = -log(sigma_dist_value_) + neg_log_sqrt_2_pi;
            inv_sigma_ = one_ / sigma_dist_value_;
        }
    }

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
        std::normal_distribution dist(mean(), stddev());
        return dist(gen);
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
        auto sign_and_int_bits = value_t::integer_bits;

        uint32_t random32_1 = gen();
        random32_1 = random32_1 << sign_and_int_bits;
        random32_1 = random32_1 >> sign_and_int_bits;

        uint32_t random32_2 = gen();
        random32_2 = random32_2 << sign_and_int_bits;
        random32_2 = random32_2 >> sign_and_int_bits;

        uint32_t random32_3 = gen();
        random32_3 = random32_3 << sign_and_int_bits;
        random32_3 = random32_3 >> sign_and_int_bits;

        auto sum = random32_1 + random32_2 + random32_3;
        sum = sum << 1;

        auto shifted_mean = value_t::from_raw_value(sum) - value_t(3);

        return (shifted_mean * stddev()) + mean();
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
        dist_value_t z_score = (x - mean(index)) / stddev(index);
        return exp(neg_point_five_ * z_score * z_score) / (stddev(index) * sqrt(two_pi_));
    }

    dist_value_t log_pdf(value_t x, size_t index=0)  const 
    {
        dist_value_t mu;
        if constexpr (std::is_base_of_v<expr::ConstantBaseType, mean_type>) {
            mu = mu_dist_value_;
        } else {
            mu = static_cast<dist_value_t>(mean(index));
        }

        if constexpr (std::is_base_of_v<expr::ConstantBaseType, stddev_type>) {
            dist_value_t z_score = (static_cast<dist_value_t>(x) - mu) * inv_sigma_;
            return neg_point_five_ * z_score * z_score + C_;
        } else {
            dist_value_t sigma = static_cast<dist_value_t>(stddev(index));

            if (sigma != mem_sigma) {
                mem_sigma = sigma;
                mem_C = -log(sigma) + neg_log_sqrt_2_pi;
                mem_inv_sigma = one_ / sigma;
            }

            dist_value_t z_score = (static_cast<dist_value_t>(x) - mu) * mem_inv_sigma;
            return neg_point_five_ * z_score * z_score + mem_C;
        }
    }

    value_t mean(size_t index=0) const { return mean_.get_value(index);}
    value_t stddev(size_t index=0) const { return stddev_.get_value(index);}
    value_t min() const { return std::numeric_limits<value_t>::lowest(); }
    value_t max() const { return std::numeric_limits<value_t>::max(); }


    std::set<boost::uuids::uuid> getDeps() {
        std::set<boost::uuids::uuid> used_vars; 
        std::set<boost::uuids::uuid> mean_used_vars = mean_.getDeps(); //runtime polymorphism: whatever class mean_ is should have it's own getDeps() function
        used_vars.insert(mean_used_vars.begin(),mean_used_vars.end());
        std::set<boost::uuids::uuid> std_dev_used_vars = stddev_.getDeps();
        used_vars.insert(std_dev_used_vars.begin(),std_dev_used_vars.end());
        return used_vars;
    }


    DeterministicInterval getInterval() {
        DeterministicInterval mean_interval = mean_.getInterval();
        DeterministicInterval stddev_interval = stddev_.getInterval();
        return DeterministicInterval(mean_interval.lower-(6.*stddev_interval.upper),mean_interval.upper+(6.*stddev_interval.upper));
    }


    DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval) {
        DeterministicInterval mean_interval = mean_.getInterval();
        biggestInterval = union_(biggestInterval,mean_interval);

        DeterministicInterval stddev_interval = stddev_.getInterval();
        biggestInterval = union_(biggestInterval,stddev_interval);

        DeterministicInterval x1 = sub(dataInterval,mean_interval);
        biggestInterval = union_(biggestInterval,x1);

        DeterministicInterval z_score_interval = div(x1,stddev_interval);
        biggestInterval = union_(biggestInterval,z_score_interval);

        DeterministicInterval z_score_square_interval = square(z_score_interval);
        biggestInterval = union_(biggestInterval,z_score_square_interval);

        DeterministicInterval two_pi_interval = constant_(double(6.28318530718));
        biggestInterval = union_(biggestInterval,two_pi_interval);

        DeterministicInterval std_dev_square_interval = square(stddev_interval);
        biggestInterval = union_(biggestInterval,std_dev_square_interval);

        DeterministicInterval arg_interval = mult(std_dev_square_interval,two_pi_interval);
        biggestInterval = union_(biggestInterval,arg_interval);

        DeterministicInterval log_interval = log_(arg_interval);
        biggestInterval = union_(biggestInterval,log_interval);

        DeterministicInterval sum_interval = add(log_interval,z_score_square_interval);
        biggestInterval = union_(biggestInterval,sum_interval);

        DeterministicInterval LikelihoodInterval = scale(-0.5,sum_interval);
        biggestInterval = union_(biggestInterval,LikelihoodInterval);
        
        return LikelihoodInterval;
    }

private:
    mean_type mean_;
    stddev_type stddev_;

    mutable dist_value_t mem_sigma = dist_value_t(0);
    mutable dist_value_t mem_C = dist_value_t(0);
    mutable dist_value_t mem_inv_sigma = dist_value_t(0);
   
    dist_value_t neg_log_sqrt_2_pi = dist_value_t(-0.918938533);
    dist_value_t neg_point_five_ = dist_value_t(-0.5);
    dist_value_t two_pi_ = dist_value_t(2 * M_PI);
    dist_value_t two_ = dist_value_t(2);
    dist_value_t one_ = dist_value_t(1);

    // precomputed constants when mean or stddev is constant
    dist_value_t mu_dist_value_;
    dist_value_t sigma_dist_value_;
    dist_value_t C_;
    dist_value_t inv_sigma_;
};

} // namespace expr
} // namespace ppl
