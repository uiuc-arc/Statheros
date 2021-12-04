#pragma once
#include <chrono>
#include <random>
//#include <fastad>
#include <autoppl/expression/model/model_utils.hpp>
#include <autoppl/util/var_traits.hpp>

#define AUTOPPL_MH_UNKNOWN_VALUE_TYPE_ERROR \
    "Unknown value type: must be convertible to util::disc_param_t " \
    "such as uint64_t or util::cont_param_t such as double."

namespace ppl {
namespace mcmc {

/**
 * Get current time in milliseconds for random seeding.
 */
inline size_t random_seed()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace mcmc
} // namespace ppl
