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
    std::cout << "probability,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed> param_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            
            std::vector<Fixed> param_samples(NUM_SAMPLES);

            ppl::Data<Fixed> is_success1 {Fixed(1)}; // location 1
            ppl::Data<Fixed> is_success2 {Fixed(1)}; // location 2
            ppl::Data<Fixed> is_success3 {Fixed(1)}; // location 3
            ppl::Data<Fixed> is_success4 {Fixed(1)}; // location 4
            ppl::Data<Fixed> is_success5 {Fixed(1)}; // location 5
            ppl::Data<Fixed> is_success6 {Fixed(1)}; // location 6
            ppl::Data<Fixed> is_success7 {Fixed(1)}; // location 7
            ppl::Data<Fixed> is_success8 {Fixed(1)}; // location 8
            ppl::Data<Fixed> is_success9 {Fixed(1)}; // location 9
            ppl::Data<Fixed> is_success10 {Fixed(1)}; // location 10
            ppl::Param<Fixed> param {param_samples.data()};

            auto model = (
                param |= ppl::uniform(Fixed(0.), Fixed(1.)),
                is_success1 |= ppl::bernoulli(param),
                is_success2 |= ppl::bernoulli(param),
                is_success3 |= ppl::bernoulli(param),
                is_success4 |= ppl::bernoulli(param),
                is_success5 |= ppl::bernoulli(param),
                is_success6 |= ppl::bernoulli(param),
                is_success7 |= ppl::bernoulli(param),
                is_success8 |= ppl::bernoulli(param),
                is_success9 |= ppl::bernoulli(param),
                is_success10 |= ppl::bernoulli(param)
            );

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto param_mean = ppl::math::mean(param_samples);

            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            param_means[i] = param_mean;
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
        std::cout << param_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
