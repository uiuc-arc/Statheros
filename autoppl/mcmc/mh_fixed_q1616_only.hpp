#pragma once
#include <algorithm>
#include <array>
#include <autoppl/util/logging.hpp>
#include <autoppl/util/traits.hpp>
#include <autoppl/variable.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <autoppl/mcmc/sampler_tools.hpp>

#include <fpm/fixed.hpp>
#include <autoppl/math/math.hpp>


using fixed_point = fpm::fixed_16_16;

namespace ppl {
namespace mcmc {


template <bool IsContinuous, class = void>
struct value_to_param
{};
template <bool IsContinuous>
struct value_to_param<IsContinuous, std::enable_if_t<!IsContinuous>>
{
    using type = util::disc_param_t;
};
template <bool IsContinuous>
struct value_to_param<IsContinuous, std::enable_if_t<IsContinuous>>
{
    using type = util::cont_param_t;
};
template <bool IsContinuous>
using value_to_param_t = typename value_to_param<IsContinuous>::type;

/**
 * Data structure to keep track of candidates in metropolis-hastings.
 */
struct MHData
{
    ppl::unique_variant_t<std::variant<util::cont_param_t, util::disc_param_t>> next;
};

template <class ModelType, class RGenType, class Iter>
inline void mh__(ModelType& model,
                 Iter params_it,
                 RGenType& gen,
                 size_t n_sample,
                 size_t warmup,
                 fixed_point curr_log_pdf,
                 fixed_point alpha,
                 fixed_point stddev)
{
    
    // if we want a fixed-point probability, then we write fixed_point::from_raw_value(unif_prob_sampler(gen))
    fixed_point zero_fp(0);  
    fixed_point one_fp(1);
    std::uniform_int_distribution unif_prob_sampler(zero_fp.raw_value(), one_fp.raw_value());

    auto logger = util::ProgressLogger(n_sample + warmup, "MetropolisHastings");

    for (size_t iter = 0; iter < n_sample + warmup; ++iter) {
        logger.printProgress(iter);

        size_t n_swaps = 0;                     // during candidate sampling, if sample out-of-bounds,
                                                // traversal will prematurely return and n_swaps < n_params
        bool early_reject = false;              // indicate early sample reject
        auto log_alpha = -curr_log_pdf;

        // generate next candidates and place them in parameter
        // variables as next values; update log_alpha
        // The old values are temporary stored in the params vector.
        auto get_candidate = [=, &n_swaps, &early_reject, &gen](auto& eq_node) mutable {
            if (early_reject) return;

            auto& var = eq_node.get_variable();
            using var_t = std::decay_t<decltype(var)>;
            using value_t = typename util::var_traits<var_t>::value_t;

#if __cplusplus <= 201703L
            if constexpr (util::is_param_v<var_t>) {
#else
            if constexpr (util::param<var_t>) {
#endif
                auto curr = var.get_value(0);
                const auto& dist = eq_node.get_distribution();

                const auto is_cont = eq_node.is_continuous_dist();
                const auto is_fixed_point = ppl::is_fixed_point_v<value_t>;

                // Old: Choose either continuous or discrete sampler depending on value_t
                /* CHANGED HERE */
                // choose sampler depending is_fixed_point and is_cont instead
                if constexpr (!is_fixed_point && !is_cont) {
                    std::discrete_distribution disc_sampler({alpha, 1-2*alpha, alpha});
                    auto cand = disc_sampler(gen) - 1 + curr; // new candidate in curr + [-1, 0, 1]
                    // TODO: refactor common logic
                    if (dist.min() <= cand && cand <= dist.max()) { // if within dist bound
                        var.set_value(cand); 
                        ++n_swaps;
                    }
                    else { early_reject = true; return; }
                } else if constexpr (!is_fixed_point && is_cont) {
                    std::normal_distribution norm_sampler(static_cast<double>(curr), stddev);
                    auto cand = norm_sampler(gen); 
                    if (dist.min() <= cand && cand <= dist.max()) { // if within dist bound
                        var.set_value(cand); 
                        ++n_swaps;
                    }
                    else { early_reject = true; return; }
                
                // use different proposal distributions for discrete / continuous distributions
                } else if constexpr (is_fixed_point && !is_cont) {
                    std::discrete_distribution disc_sampler({1, 135, 1}); // same ratio as {alpha, 1-2*alpha, alpha}
                    auto cand = value_t(disc_sampler(gen)) - value_t(1) + value_t(curr); // TODO: pull out constant

                    if (dist.min() <= cand && cand <= dist.max()) { // if within dist bound
                        var.set_value(cand); 
                        ++n_swaps;
                    }
                    else { early_reject = true; return; }
                } else if constexpr (is_fixed_point && is_cont) {
                    using namespace math;  
                    value_t zero(0);  
                    value_t two(2);  

                    auto u1 = value_t::from_raw_value(unif_prob_sampler(gen));
                    auto u2 = value_t::from_raw_value(unif_prob_sampler(gen));

                    auto negtwo_log = -two * log(u1);
                    while (negtwo_log < zero) {
                        u1 = value_t::from_raw_value(unif_prob_sampler(gen));
                        negtwo_log = -two * log(u1);
                    }

                    auto z0 = sqrt(negtwo_log) * cos(two * value_t::pi() * u2);
                    auto cand = z0 * value_t(stddev) + value_t(curr);

                    if (dist.min() <= cand && cand <= dist.max()) { 
                        var.set_value(cand); 
                        ++n_swaps;
                    }
                    else { early_reject = true; return; }
                } else {
                    static_assert(!(ppl::is_arithmetic_v<value_t>),
                                    AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR);
                }

                using converted_value_t = value_to_param_t<is_cont>;
                params_it->next = static_cast<converted_value_t>(curr);
                ++params_it;
            }
        };
        model.traverse(get_candidate);

        if (early_reject) {

            auto add_to_storage = [=, &n_swaps](auto& eq_node) mutable {
                auto& var = eq_node.get_variable();
                using var_t = std::decay_t<decltype(var)>;
                using value_t = typename util::var_traits<var_t>::value_t;
#if __cplusplus <= 201703L
                if constexpr (util::is_param_v<var_t>) {
#else
                if constexpr (util::param<var_t>) {
#endif
                    if (n_swaps) {
                        const auto is_cont = eq_node.is_continuous_dist();
                        using converted_value_t = value_to_param_t<is_cont>;
                        var.set_value(*std::get_if<converted_value_t>(&params_it->next));
                        ++params_it;
                        --n_swaps;
                    }
                    if (iter >= warmup) {
                        auto storage = var.get_storage();
                        storage[iter - warmup] = var.get_value(0);
                    }
                } 
            };
            model.traverse(add_to_storage);
            continue;
        }

        // compute next candidate log pdf and update log_alpha
        auto cand_log_pdf = model.log_pdf();
        log_alpha += cand_log_pdf;

        bool accept = (ppl::math::log(fixed_point::from_raw_value(unif_prob_sampler(gen))) <= log_alpha);

        // If accept, "current" sample for next iteration is already in the variables
        // so simply append to storage.
        // Otherwise, "current" sample for next iteration must be moved back from 
        // params vector into variables.
        auto add_to_storage = [=](auto& eq_node) mutable {
            auto& var = eq_node.get_variable();
            using var_t = std::decay_t<decltype(var)>;
            using value_t = typename util::var_traits<var_t>::value_t;
#if __cplusplus <= 201703L
            if constexpr(util::is_param_v<var_t>) {
#else
            if constexpr(util::param<var_t>) {
#endif
                if (!accept) {
                    const auto is_cont = eq_node.is_continuous_dist();
                    using converted_value_t = value_to_param_t<is_cont>;
                    var.set_value(*std::get_if<converted_value_t>(&params_it->next));
                    ++params_it;
                }
                if (iter >= warmup) {
                    auto storage = var.get_storage();
                    storage[iter - warmup] = var.get_value(0);
                }
            } 
        };
        model.traverse(add_to_storage);

        // update current log pdf for next iteration
        if (accept) curr_log_pdf = cand_log_pdf;
    }

    std::cout << std::endl;
}

} // namespace mcmc

template <class ModelType>
inline void mh(ModelType& model,
               size_t n_sample,
               size_t warmup = 1000,
               fixed_point stddev = fixed_point(1.0),
               fixed_point alpha = fixed_point(0.25),
               size_t seed = mcmc::random_seed()
               )
{
    using data_t = mcmc::MHData;
    
    // set-up auxiliary tools 
    std::mt19937 gen(seed);
    size_t n_params = 0;


    fixed_point curr_log_pdf = fixed_point(0.);  // current log pdf

    // 1. initialize parameters with values in valid range
    // - discrete valued params sampled uniformly within the distribution range
    // - continuous valued params sampled uniformly within the intersection range
    //   of distribution min and max and [-initial_radius, initial_radius]
    // 2. update n_params with number of parameters
    // 3. compute current log-pdf
    auto init_params = [&](auto& eq_node) {
        auto& var = eq_node.get_variable();
        const auto& dist = eq_node.get_distribution();

        using var_t = std::decay_t<decltype(var)>;
        using value_t = typename util::var_traits<var_t>::value_t;

        constexpr value_t initial_radius = value_t(5.0);
        const auto is_fixed_point = ppl::is_fixed_point_v<value_t>;
        const auto is_cont = eq_node.is_continuous_dist();

#if __cplusplus <= 201703L
        if constexpr (util::is_param_v<var_t>) {
#else
        if constexpr (util::param<var_t>) {
#endif      

            // choose sampler depending on is_fixed_point and is_cont instead
            if constexpr (!is_fixed_point && !is_cont) {
                std::uniform_int_distribution init_sampler(dist.min(), dist.max());
                var.set_value(init_sampler(gen));
            } else if constexpr (!is_fixed_point && is_cont) {
                std::uniform_real_distribution init_sampler(
                        std::max(dist.min(), -initial_radius), 
                        std::min(dist.max(), initial_radius)
                        );
                var.set_value(init_sampler(gen));
            
            // use different distributions for discrete / continuous distributions
            } else if constexpr (is_fixed_point && !is_cont) {
                std::uniform_int_distribution init_sampler(dist.min().raw_value(), dist.max().raw_value());
                var.set_value(ppl::math::floor(value_t::from_raw_value(init_sampler(gen))));
            } else if constexpr (is_fixed_point && is_cont) {
                std::uniform_int_distribution init_sampler(
                        ppl::math::max(dist.min(), -initial_radius).raw_value(), 
                        ppl::math::min(dist.max(), initial_radius).raw_value()
                        );
                var.set_value(value_t::from_raw_value(init_sampler(gen)));
            } else {
                static_assert(!(ppl::is_arithmetic_v<value_t>), 
                              AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR);
            }
            ++n_params;
        }
        curr_log_pdf += dist.log_pdf(var);
    };
    model.traverse(init_params);

    std::vector<data_t> params(n_params);   // vector of parameter-related data with candidate
    mcmc::mh__(model,
               params.begin(),
               gen,
               n_sample,
               warmup,
               curr_log_pdf,
               alpha,
               stddev);
}

} // namespace ppl
