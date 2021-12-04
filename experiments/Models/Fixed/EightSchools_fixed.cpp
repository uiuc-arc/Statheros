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
    std::cout << "mu,tau,theta_std_1,theta_std_2,theta_std_3,theta_std_4,theta_std_5,"
              << "theta_std_6,theta_std_7,theta_std_8,acceptance_ratio,time_elapsed_us,"
              << "start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed>  mu_means(NUM_ITERS),
                        tau_means(NUM_ITERS),
                        theta_std_1_means(NUM_ITERS),
                        theta_std_2_means(NUM_ITERS),
                        theta_std_3_means(NUM_ITERS),
                        theta_std_4_means(NUM_ITERS),
                        theta_std_5_means(NUM_ITERS),
                        theta_std_6_means(NUM_ITERS),
                        theta_std_7_means(NUM_ITERS),
                        theta_std_8_means(NUM_ITERS);

    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
	        std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<Fixed>  mu_samples(NUM_SAMPLES), 
                                tau_samples(NUM_SAMPLES),
                                theta_std_1_samples(NUM_SAMPLES),
                                theta_std_2_samples(NUM_SAMPLES),
                                theta_std_3_samples(NUM_SAMPLES),
                                theta_std_4_samples(NUM_SAMPLES),
                                theta_std_5_samples(NUM_SAMPLES),
                                theta_std_6_samples(NUM_SAMPLES),
                                theta_std_7_samples(NUM_SAMPLES),
                                theta_std_8_samples(NUM_SAMPLES);

            ppl::Param<Fixed>   mu {mu_samples.data()},
                                tau {tau_samples.data()},
                                theta_std_1 {theta_std_1_samples.data()},
                                theta_std_2 {theta_std_2_samples.data()},
                                theta_std_3 {theta_std_3_samples.data()},
                                theta_std_4 {theta_std_4_samples.data()},
                                theta_std_5 {theta_std_5_samples.data()},
                                theta_std_6 {theta_std_6_samples.data()},
                                theta_std_7 {theta_std_7_samples.data()},
                                theta_std_8 {theta_std_8_samples.data()};

            ppl::Data<Fixed>    y1 {28}, 
                                y2 {8}, 
                                y3 {-3}, 
                                y4 {7}, 
                                y5 {-1}, 
                                y6 {1}, 
                                y7 {18},
                                y8 {12};

            std::vector<Fixed> sigma = {15, 10, 16, 11, 9, 11, 10, 18};

            auto model = (
                mu |= ppl::uniform(Fixed(-20), Fixed(20)),
                tau |= ppl::uniform(Fixed(0), Fixed(20)),

                theta_std_1 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_2 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_3 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_4 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_5 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_6 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_7 |= ppl::normal(Fixed(0), Fixed(1)),
                theta_std_8 |= ppl::normal(Fixed(0), Fixed(1)),

                y1 |= ppl::normal(mu + tau * theta_std_1, sigma[0]),
                y2 |= ppl::normal(mu + tau * theta_std_2, sigma[1]),
                y3 |= ppl::normal(mu + tau * theta_std_3, sigma[2]),
                y4 |= ppl::normal(mu + tau * theta_std_4, sigma[3]),
                y5 |= ppl::normal(mu + tau * theta_std_5, sigma[4]),
                y6 |= ppl::normal(mu + tau * theta_std_6, sigma[5]),
                y7 |= ppl::normal(mu + tau * theta_std_7, sigma[6]),
                y8 |= ppl::normal(mu + tau * theta_std_8, sigma[7])
            );

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

	        std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto mu_mean = ppl::math::mean(mu_samples);
            auto tau_mean = ppl::math::mean(tau_samples);
            auto theta_std_1_mean = ppl::math::mean(theta_std_1_samples);
            auto theta_std_2_mean = ppl::math::mean(theta_std_2_samples);
            auto theta_std_3_mean = ppl::math::mean(theta_std_3_samples);
            auto theta_std_4_mean = ppl::math::mean(theta_std_4_samples);
            auto theta_std_5_mean = ppl::math::mean(theta_std_5_samples);
            auto theta_std_6_mean = ppl::math::mean(theta_std_6_samples);
            auto theta_std_7_mean = ppl::math::mean(theta_std_7_samples);
            auto theta_std_8_mean = ppl::math::mean(theta_std_8_samples);

   	        std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            mu_means[i] = mu_mean;
            tau_means[i] = tau_mean;
            theta_std_1_means[i] = theta_std_1_mean;
            theta_std_2_means[i] = theta_std_2_mean;
            theta_std_3_means[i] = theta_std_3_mean;
            theta_std_4_means[i] = theta_std_4_mean;
            theta_std_5_means[i] = theta_std_5_mean;
            theta_std_6_means[i] = theta_std_6_mean;
            theta_std_7_means[i] = theta_std_7_mean;
            theta_std_8_means[i] = theta_std_8_mean;
            acceptance_ratios[i] = acceptance_ratio;
            t_starts[i] = t_start;
            t_ends[i] = t_end;
            t_before_mhs[i] = t_before_mh;
            t_after_mhs[i] = t_after_mh;
        } catch(...) {
            std::cout << "x,x,x,x,x,x,x,x,x,x,x,x" << std::endl;
        }

        globals::num_accepts = 0;
        globals::num_samples = 0;
    }

    for (int i = 0; i < NUM_ITERS; ++i) {
        std::cout << mu_means[i] << ","
                << tau_means[i] << ","
                << theta_std_1_means[i] << ","
                << theta_std_2_means[i] << ","
                << theta_std_3_means[i] << ","
                << theta_std_4_means[i] << ","
                << theta_std_5_means[i] << ","
                << theta_std_6_means[i] << ","
                << theta_std_7_means[i] << ","
                << theta_std_8_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
