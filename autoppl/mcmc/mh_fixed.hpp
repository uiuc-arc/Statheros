#pragma once
#include <algorithm>
#include <autoppl/mcmc/sampler_tools.hpp>
#include <autoppl/util/traits.hpp>
#include <autoppl/variable.hpp>
#include <random>
#include <vector>

#include <autoppl/math/math.hpp>
#include <autoppl/globals.hpp>

#include <fpm/ios.hpp>

#define AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR \
    "Unknown value type: must be convertible to util::disc_param_t " \
    "or util::cont_param_t."

/* See fixed_point_changelog.txt for explanation of changes */

/* CHANGE D */
// by default, assumes that all distributions have the same dist_value_t
// if not, that is okay, as types will automatically be casted/converted
using dist_value_t = ppl::util::dist_value_t_default;
using cont_param_t = ppl::util::cont_param_t;

namespace ppl {
namespace mcmc {

/**
 * Convert ValueType (which could be any fixed-point type the user uses
 * when writing their model) to either util::cont_param_t or util::disc_param_t
 */
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

    // include all possible VALUE TYPES (value_t) in the inner std::variant
    ppl::unique_variant_t<std::variant<util::cont_param_t, 
                                        util::disc_param_t>> next;
};

template <class ModelType, class RGenType, class Iter>
inline void mh__(ModelType& model,
                 Iter params_it,
                 RGenType& gen,
                 size_t n_sample,
                 size_t warmup,
                 dist_value_t curr_log_pdf, /* CHANGE D */
                 size_t alpha, /* CHANGE C */
                 cont_param_t stddev) /* CHANGE D */
{
    // auto logger = util::ProgressLogger(n_sample + warmup, "MetropolisHastings");
    bool err_flag = false;

    dist_value_t zero_dist_value(0);
    dist_value_t one_dist_value(1);

    for (size_t iter = 0; iter < n_sample + warmup; ++iter) {
        ++globals::num_samples;

        // logger.printProgress(iter);

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

            if constexpr (util::is_param_v<var_t>) {
                auto curr = var.get_value(0);
                const auto& dist = eq_node.get_distribution();
                
                // Choose either continuous or discrete sampler depending on distribution
                if constexpr (std::is_base_of_v<ppl::expr::EqNodeDisc, std::remove_reference_t<decltype(eq_node)>>) {
                    /* CHANGE C */
                    // original was {1, 1 - 2*alpha, 1}, where 0 <= alpha <= 0.5
                    // scale up to use integers instead, where 0 <= alpha <= 50
                    // original code: std::discrete_distribution disc_sampler({100, 100 - 2*alpha, 100});
                    // below: modified to not use doubles, use uniform_int_distribution instead
                    
                    const std::array<int, 3> weights({alpha, 100 - (2 * alpha), alpha});
                    int upper_bound = std::accumulate(weights.begin(), weights.end(), 0);
                    std::uniform_int_distribution disc_sampler(1, upper_bound);

                    int sampled_value = disc_sampler(gen);
                    int i;  // this is analagous to the original output when sampling std::discrete_distribution
                    for (i = 0; weights[i] < sampled_value; ++i) {
                        sampled_value -= weights[i];
                    }

                    --i;  // changes range from [0, 1, 2] -> [-1, 0, 1]
                    auto cand = value_t(i) + value_t(curr);

                    if (dist.min() <= cand && cand <= dist.max()) { // if within dist bound
                        var.set_value(cand); 
                        ++n_swaps;
                    } else { 
                        early_reject = true; return; 
                    }
                } else if constexpr (std::is_base_of_v<ppl::expr::EqNodeCont, std::remove_reference_t<decltype(eq_node)>>) {
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
                    auto cand = (shifted_mean * value_t(stddev)) + value_t(curr);

                    if (dist.min() <= cand && cand <= dist.max()) { // if within dist bound
                        var.set_value(cand); 
                        ++n_swaps;
                    } else {
                        early_reject = true; return;
                    }
		        } else {
                    static_assert(!ppl::is_fixed_point_v<value_t>, "Value is not a fixed-point number!");
                    static_assert(!(ppl::is_arithmetic_v<value_t>),
                                    AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR);

                }

                // move old value into params
                using converted_value_t = value_to_param_t<std::is_base_of_v<ppl::expr::EqNodeCont, decltype(eq_node)>>;
                params_it->next = static_cast<converted_value_t>(curr);
                ++params_it;
            }
        };
        model.traverse(get_candidate);

        if (early_reject) {
	        ++globals::early_exits;

            // swap back original params only up until when candidate was out of bounds.
            auto add_to_storage = [=, &n_swaps](auto& eq_node) mutable {
                auto& var = eq_node.get_variable();
                using var_t = std::decay_t<decltype(var)>;
                using value_t = typename util::var_traits<var_t>::value_t;

                if constexpr (util::is_param_v<var_t>) {
                    if (n_swaps) {
                        using converted_value_t = value_to_param_t<std::is_base_of_v<ppl::expr::EqNodeCont, decltype(eq_node)>>;
                        var.set_value(*std::get_if<converted_value_t>(&params_it->next));
                        ++params_it;
                        --n_swaps;
                    }

                    if (iter >= warmup) {
                        if (var.infer_value()) {
                            auto storage = var.get_storage();
                            storage[iter - warmup] = var.get_value(0);
                        }
                    }
                }
            };
            model.traverse(add_to_storage);
            continue;
        }

        // compute next candidate log pdf and update log_alpha
        auto cand_log_pdf = model.log_pdf();

        // check for possible overflow error that would cause incorrect inferred value
        if (!err_flag && iter >= warmup) {
            err_flag = (cand_log_pdf > zero_dist_value
                    && curr_log_pdf <= zero_dist_value
                    && (cand_log_pdf - globals::MAX_DIST_VALUE_T  > curr_log_pdf));
        }

        // // Uncomment to see individual errors before/after burn-in
        // if (cand_log_pdf > zero_dist_value
        //         && curr_log_pdf <= zero_dist_value
        //         && (cand_log_pdf - globals::MAX_DIST_VALUE_T  > curr_log_pdf)) {
        //     if (iter >= warmup) {
        //         globals::err_after_warmup++;
        //     } else {
        //         globals::err_before_warmup++;
        //     }
        // }

        log_alpha += cand_log_pdf;

        /* CHANGE B */
        auto sign_and_int_bits = dist_value_t::integer_bits;
        uint32_t random32 = gen();
        random32 = random32 << sign_and_int_bits;
        random32 = random32 >> sign_and_int_bits;
        dist_value_t uniform_0_1_sample = dist_value_t::from_raw_value(random32);

        bool accept = (ppl::math::log(uniform_0_1_sample) <= log_alpha);

        // for experimental purposes
        if (accept) {
            ++globals::num_accepts;
        }

        // If accept, "current" sample for next iteration is already in the variables
        // so simply append to storage.
        // Otherwise, "current" sample for next iteration must be moved back from 
        // params vector into variables.
        auto add_to_storage = [=](auto& eq_node) mutable {
            auto& var = eq_node.get_variable();
            using var_t = std::decay_t<decltype(var)>;
            using value_t = typename util::var_traits<var_t>::value_t;

            if constexpr(util::is_param_v<var_t>) {
                if (!accept) {
                    using converted_value_t = value_to_param_t<std::is_base_of_v<ppl::expr::EqNodeCont, decltype(eq_node)>>;
                    var.set_value(*std::get_if<converted_value_t>(&params_it->next));
                    ++params_it;
                }

                if (iter >= warmup) {
                    if (var.infer_value()) {
                        auto storage = var.get_storage();
                        storage[iter - warmup] = var.get_value(0);
                    }
                }
            } 
        };
        model.traverse(add_to_storage);

        // update current log pdf for next iteration
        if (accept) curr_log_pdf = cand_log_pdf;  
    }

    // std::cout << std::endl;
    //std::cout << "Error? " << err_flag << std::endl;
}

} // namespace mcmc


template <class ModelType>
inline void mh(ModelType& model,
               size_t n_sample,
               size_t warmup = 1000,
               cont_param_t stddev = cont_param_t(1), /* CHANGE D */
               size_t alpha = 25, /* CHANGE C */
               size_t seed = mcmc::random_seed()
               )
{
    using data_t = mcmc::MHData;
    
    // set-up auxiliary tools 
    std::mt19937 gen(seed);
    size_t n_params = 0;

    /* CHANGE D */
    dist_value_t curr_log_pdf(0);  // current log pdf

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

        constexpr value_t initial_radius = value_t(5);
        auto sign_and_int_bits = value_t::integer_bits;

        if constexpr (util::is_param_v<var_t>) {     
            if constexpr (std::is_base_of_v<ppl::expr::EqNodeDisc, std::remove_reference_t<decltype(eq_node)>>) {
                uint32_t random32 = gen();
                random32 = random32 << sign_and_int_bits;
                random32 = random32 >> sign_and_int_bits;
                value_t uniform_0_1_sample = value_t::from_raw_value(random32);
                value_t scaled_sample = uniform_0_1_sample * (dist.max() - dist.min()) + dist.min();

                var.set_value(ppl::math::floor(scaled_sample));
            } else if constexpr (std::is_base_of_v<ppl::expr::EqNodeCont, std::remove_reference_t<decltype(eq_node)>>) {
                /* CHANGE E */
                // Important!! Subtle point: need to cast distribution's value_t to var's value_t

                uint32_t random32 = gen();
                random32 = random32 << sign_and_int_bits;
                random32 = random32 >> sign_and_int_bits;
                value_t uniform_0_1_sample = value_t::from_raw_value(random32);
                value_t min_value = ppl::math::max(static_cast<value_t>(dist.min()), -initial_radius);
                value_t max_value = ppl::math::min(static_cast<value_t>(dist.max()), initial_radius);
                value_t scaled_sample = uniform_0_1_sample * (max_value - min_value) + min_value;

                var.set_value(scaled_sample);
            } else {
                static_assert(!ppl::is_fixed_point_v<value_t>, "Value is not a fixed-point number!");
                static_assert(!(ppl::is_arithmetic_v<value_t>), 
                              AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR);
                // TODO: enforce that all distributions extend from either discrete or cont. distribution base class
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
