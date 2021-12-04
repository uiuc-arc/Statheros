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
    std::cout << "Skill1,Skill2,Skill3,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed>  skill1_means(NUM_ITERS),
                        skill2_means(NUM_ITERS),
                        skill3_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            
            std::vector<Fixed>  skill1_samples(NUM_SAMPLES), 
                                skill2_samples(NUM_SAMPLES),
                                skill3_samples(NUM_SAMPLES),
                                perf1_samples(NUM_SAMPLES),
                                perf2_samples(NUM_SAMPLES),
                                perf3_samples(NUM_SAMPLES);

            ppl::Data<Fixed> game1 {1.0};
            ppl::Data<Fixed> game2 {1.0};
            ppl::Data<Fixed> game3 {1.0};

            ppl::Param<Fixed> skill1 {skill1_samples.data()};
            ppl::Param<Fixed> skill2 {skill2_samples.data()};
            ppl::Param<Fixed> skill3 {skill3_samples.data()};
            ppl::Param<Fixed> perf1 {perf1_samples.data()};
            ppl::Param<Fixed> perf2 {perf2_samples.data()};
            ppl::Param<Fixed> perf3 {perf3_samples.data()};

            auto model = (
                skill1 |= ppl::normal(Fixed(100.), Fixed(10.)),
                skill2 |= ppl::normal(Fixed(100.), Fixed(10.)),
                skill3 |= ppl::normal(Fixed(100.), Fixed(10.)),

                perf1 |= ppl::normal(skill1, Fixed(15.)),
                perf2 |= ppl::normal(skill2, Fixed(15.)),
                perf3 |= ppl::normal(skill3, Fixed(15.)),

                game1 |= ppl::bernoulli((perf1>perf2)),
                game2 |= ppl::bernoulli((perf2>perf3)),
                game3 |= ppl::bernoulli((perf1>perf3))
            );

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto skill1_mean = ppl::math::mean(skill1_samples);
            auto skill2_mean = ppl::math::mean(skill2_samples);
            auto skill3_mean = ppl::math::mean(skill3_samples);

            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            skill1_means[i] = skill1_mean;
            skill2_means[i] = skill2_mean;
            skill3_means[i] = skill3_mean;
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
        std::cout << skill1_means[i] << ","
                << skill2_means[i] << ","
                << skill3_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
