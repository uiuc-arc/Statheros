#include <chrono>
#include <ctime>   
#include <iostream>
#include <string>
#include <vector>

#include <autoppl/math/math.hpp>
#include <autoppl/mcmc/mh.hpp>
#include <autoppl/expr_builder.hpp>
#include <autoppl/config.hpp>
#include <autoppl/globals.hpp>

using cont_t = ppl::util::cont_param_t;

using namespace std::chrono;

int main() {
    std::cout << "group1_mu,group2_mu,group3_mu,common_sigma,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<cont_t>  mu1_means(NUM_ITERS),
                        mu2_means(NUM_ITERS),
                        mu3_means(NUM_ITERS),
                        common_sigma_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<cont_t> mu1_samples(NUM_SAMPLES),
                                mu2_samples(NUM_SAMPLES),
                                mu3_samples(NUM_SAMPLES),
                                sigma_samples(NUM_SAMPLES);

            ppl::Data<cont_t> p1_samples {cont_t(90), cont_t(95), cont_t(100)}; //person 1
            ppl::Data<cont_t> p2_samples {cont_t(105), cont_t(110), cont_t(115)}; //person 2
            ppl::Data<cont_t> p3_samples {cont_t(150), cont_t(155), cont_t(160)}; //person 3

            ppl::Param<cont_t> mu_1 {mu1_samples.data()};
            ppl::Param<cont_t> mu_2 {mu2_samples.data()};
            ppl::Param<cont_t> mu_3 {mu3_samples.data()};
            ppl::Param<cont_t> common_sigma {sigma_samples.data()};

            auto model = (
                common_sigma |= ppl::uniform(0.5, 100),
                mu_1 |= ppl::uniform(0, 300),
                mu_2 |= ppl::uniform(0, 300),
                mu_3 |= ppl::uniform(0, 300),
                p1_samples |= ppl::normal(mu_1, common_sigma),
                p2_samples |= ppl::normal(mu_2, common_sigma),
                p3_samples |= ppl::normal(mu_3, common_sigma)
            );

            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples, 3.0);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto mu1_mean = ppl::math::mean(mu1_samples);
            auto mu2_mean = ppl::math::mean(mu2_samples);
            auto mu3_mean = ppl::math::mean(mu3_samples);
            auto common_sigma_mean = ppl::math::mean(sigma_samples);

            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            mu1_means[i] = mu1_mean;
            mu2_means[i] = mu2_mean;
            mu3_means[i] = mu3_mean;
            common_sigma_means[i] = common_sigma_mean;
            acceptance_ratios[i] = acceptance_ratio;
            t_starts[i] = t_start;
            t_ends[i] = t_end;
            t_before_mhs[i] = t_before_mh;
            t_after_mhs[i] = t_after_mh;
        } catch(...) {
            std::cout << "x,x,x,x,x,x" << std::endl;
        }

        globals::num_accepts = 0;
        globals::num_samples = 0;
    }

    for (int i = 0; i < NUM_ITERS; ++i) {
        std::cout << mu1_means[i] << ","
                << mu2_means[i] << ","
                << mu3_means[i] << ","
                << common_sigma_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
