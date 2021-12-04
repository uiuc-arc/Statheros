#pragma once
// compile with -D FP if using Fixed-Point numbers by default
#include <fpm/fixed.hpp>

namespace ppl {
namespace util {

#ifdef FP

    #ifndef PREC
        #define PREC 16
    #endif
    #ifndef DIST_VALUE_PREC
        #define DIST_VALUE_PREC 16
    #endif

    using cont_param_t = fpm::fixed<std::int32_t, std::int64_t, PREC>;
    using disc_param_t = fpm::fixed<std::int32_t, std::int64_t, PREC>;
    using dist_value_t_default = fpm::fixed<std::int32_t, std::int64_t, DIST_VALUE_PREC>;

    // more specific types for each distribution
    using value_t_bernoulli = fpm::fixed<std::int32_t, std::int64_t, PREC>;
    using dist_value_t_bernoulli = fpm::fixed<std::int32_t, std::int64_t, DIST_VALUE_PREC>;

#else

    #ifdef BIT32
        using cont_param_t = float;
        using disc_param_t = int32_t;
        using dist_value_t_default = float;

        // more specific types for each distribution
        using value_t_bernoulli = int32_t;
        using dist_value_t_bernoulli = float;
    #else
        using cont_param_t = double;
        using disc_param_t = int64_t;
        using dist_value_t_default = double;

        // more specific types for each distribution
        using value_t_bernoulli = int64_t;
        using dist_value_t_bernoulli = double;
    #endif

#endif

} // namespace util
} // namespace ppl
