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
    std::cout << "d_param,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed> d_param_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);


    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<Fixed> d_param_samples(NUM_SAMPLES);

            ppl::Data<Fixed> x {5.0,6.0}; 
            ppl::Param<Fixed> d_param {d_param_samples.data()};

            auto model = (
                d_param |= ppl::uniform(Fixed(0.),Fixed(10.)),
		//x |= ppl::mixture3(Fixed(0.1),ppl::uniform(Fixed(0.), Fixed(10.)),Fixed(0.8), ppl::normal(d_param, Fixed(2.)), Fixed(0.1), ppl::normal(Fixed(10.), Fixed(0.1)))
		x |= ppl::mixture(Fixed(0.1),ppl::uniform(Fixed(0.), Fixed(10.)),Fixed(0.8), ppl::normal(d_param, Fixed(2.)), Fixed(0.1), ppl::triangular(Fixed(9.),Fixed(10.), Fixed(10.)))
            );

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto d_param_mean = ppl::math::mean(d_param_samples);

            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            d_param_means[i] = d_param_mean;
            acceptance_ratios[i] = acceptance_ratio;
            t_starts[i] = t_start;
            t_ends[i] = t_end;
            t_before_mhs[i] = t_before_mh;
            t_after_mhs[i] = t_after_mh;
        } catch(...) {
            std::cout << "x,x,x" << std::endl;
        }

        globals::num_accepts = 0;
        globals::num_samples = 0;
    }

    for (int i = 0; i < NUM_ITERS; ++i) {
        std::cout << d_param_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
