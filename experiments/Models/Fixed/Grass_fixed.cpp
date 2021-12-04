#include <chrono>
#include <ctime>   
#include <iostream>
#include <string>
#include <vector>

#include <autoppl/math/math.hpp>
#include <autoppl/mcmc/mh_fixed.hpp>
#include <autoppl/expr_builder.hpp>
#include <autoppl/config.hpp>
#include <autoppl/globals.hpp>

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>

using namespace std::chrono;

int main() {
    std::cout << "cloudy,sprinkler,rain,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;

    std::vector<Fixed>  cloudy_means(NUM_ITERS),
                        sprinkler_means(NUM_ITERS),
                        rain_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
	        std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<Fixed> cloudy_samples(NUM_SAMPLES),
                            sprinkler_samples(NUM_SAMPLES),
                            rain_samples(NUM_SAMPLES);

            ppl::Param<Fixed> cloudy {cloudy_samples.data()}, 
                            sprinkler {sprinkler_samples.data()},
                            rain {rain_samples.data()};
            ppl::Data<Fixed> wet_grass;

            auto model = (
                cloudy |= ppl::bernoulli(Fixed(0.5)),
                sprinkler |= ppl::bernoulli(cloudy * Fixed(0.1) + (1 - cloudy) * Fixed(0.5)),
                rain |= ppl::bernoulli(cloudy * Fixed(0.8) + (1 - cloudy) * Fixed(0.2)),
                wet_grass |= ppl::bernoulli((sprinkler * rain) * Fixed(0.99)
                                        + (sprinkler * (1 - rain)) * Fixed(0.9)
                                        + ((1 - sprinkler) * rain) * Fixed(0.9)
                                        + ((1 - sprinkler) * (1 - rain)) * Fixed(0.01))
            );

            wet_grass.observe(1);

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

	        std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto cloudy_mean = ppl::math::mean(cloudy_samples);
            auto sprinkler_mean = ppl::math::mean(sprinkler_samples);
            auto rain_mean = ppl::math::mean(rain_samples);

   	        std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            cloudy_means[i] = cloudy_mean;
            sprinkler_means[i] = sprinkler_mean;
            rain_means[i] = rain_mean;
            acceptance_ratios[i] = acceptance_ratio;
            t_starts[i] = t_start;
            t_ends[i] = t_end;
            t_before_mhs[i] = t_before_mh;
            t_after_mhs[i] = t_after_mh;
        } catch(...) {
            std::cout << "x,x,x,x,x" << std::endl;
        }

        globals::num_accepts = 0;
        globals::num_samples = 0;
    }

    for (int i = 0; i < NUM_ITERS; ++i) {
        std::cout << cloudy_means[i] << ","
                << sprinkler_means[i] << ","
                << rain_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
