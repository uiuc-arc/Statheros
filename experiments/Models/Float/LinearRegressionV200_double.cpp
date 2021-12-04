#include <chrono>
#include <ctime>   
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <autoppl/math/math.hpp>
#include <autoppl/mcmc/mh.hpp>
#include <autoppl/expr_builder.hpp>
#include <autoppl/config.hpp>
#include <autoppl/globals.hpp>

using namespace std::chrono;

const double SIGMA = 0.5;
const double M = 2;
const double B = 6;
const unsigned NUM_DATA_SAMPLES = 200;

using cont_t = ppl::util::cont_param_t;

int main() {
    std::cout << "m,b,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;

    std::vector<cont_t> m_means(NUM_ITERS), b_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
	        std::mt19937 gen(std::time(0));
            std::vector<cont_t> x_samples, y_samples;
            for (double x = -10; x < (-10+0.1*NUM_DATA_SAMPLES); x += 0.1) {
                std::normal_distribution<double> ndist(M * x + B, SIGMA);
                y_samples.push_back(cont_t(ndist(gen)));
                x_samples.push_back(cont_t(x));
            }

		    std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<cont_t> m_samples(NUM_SAMPLES), b_samples(NUM_SAMPLES);
            ppl::Param<cont_t> m {m_samples.data()};
            ppl::Param<cont_t> b {b_samples.data()};
            ppl::Param<cont_t> sigma(SIGMA);
            ppl::Data<cont_t> x(x_samples.begin(), x_samples.end());
            ppl::Data<cont_t> y(y_samples.begin(), y_samples.end());
            
            auto model = (
                m |= ppl::uniform(cont_t(0), cont_t(10)),
                b |= ppl::uniform(cont_t(0), cont_t(10)),
                y |= ppl::normal(m * x + b, sigma)
            );


            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

	        std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
	        std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto m_mean = ppl::math::mean(m_samples);
            auto b_mean = ppl::math::mean(b_samples);

   	        std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            m_means[i] = m_mean;
            b_means[i] = b_mean;
            acceptance_ratios[i] = acceptance_ratio;
            t_starts[i] = t_start;
            t_ends[i] = t_end;
            t_before_mhs[i] = t_before_mh;
            t_after_mhs[i] = t_after_mh;
        } catch (...) {
            std::cout << "x,x,x,x" << std::endl;
        }

        globals::num_accepts = 0;
        globals::num_samples = 0;
    }

    for (int i = 0; i < NUM_ITERS; ++i) {
        std::cout << m_means[i] << ","
                << b_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_after_mhs[i].count() - t_before_mhs[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
    //std::cout << globals::time_spent_in_log << std::endl;
}
