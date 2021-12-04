#pragma once
#include <fpm/fixed.hpp>
#include <autoppl/settings.hpp>

#ifndef NUM_ITERS
constexpr int NUM_ITERS = 20;
#endif

#ifndef NUM_SAMPLES
constexpr int NUM_SAMPLES = 10000;
#endif

#ifndef BURN_IN_PERCENTAGE
constexpr int BURN_IN_PERCENTAGE = 50;
#endif

#ifdef PREC
using Fixed = fpm::fixed<int32_t, int64_t, PREC>;
#endif
