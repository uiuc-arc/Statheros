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
    std::cout << "burglary,earthquake,alarm,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed>  burglary_means(NUM_ITERS), 
                        earthquake_means(NUM_ITERS),
                        alarm_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<Fixed>  burglary_samples(NUM_SAMPLES),
                                earthquake_samples(NUM_SAMPLES),
                                alarm_samples(NUM_SAMPLES);

            ppl::Param<Fixed> 	burglary {burglary_samples.data()}, 
                                earthquake {earthquake_samples.data()},
                                alarm {alarm_samples.data()};
            ppl::Data<Fixed> john_calls, mary_calls;

            auto model = (
                burglary |= ppl::bernoulli(Fixed(0.001)),
                earthquake |= ppl::bernoulli(Fixed(0.002)),
                alarm |= ppl::bernoulli((burglary * earthquake) * Fixed(0.95)
                                        + (burglary * (1 - earthquake)) * Fixed(0.94)
                                        + ((1 - burglary) * earthquake) * Fixed(0.29)
                                        + ((1 - burglary) * (1 - earthquake)) * Fixed(0.001)),
                john_calls |= ppl::bernoulli(alarm * Fixed(0.90) + (1 - alarm) * Fixed(0.05)),
                mary_calls |= ppl::bernoulli(alarm * Fixed(0.70) + (1 - alarm) * Fixed(0.01))
            );

            john_calls.observe(1);
            mary_calls.observe(1);

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto burglary_mean = ppl::math::mean(burglary_samples);
            auto earthquake_mean = ppl::math::mean(earthquake_samples);
            auto alarm_mean = ppl::math::mean(alarm_samples);
        
            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            burglary_means[i] = burglary_mean;
            earthquake_means[i] = earthquake_mean;
            alarm_means[i] = alarm_mean;
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
        std::cout << burglary_means[i] << ","
                << earthquake_means[i] << ","
                << alarm_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
