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

const double M = 0.5;  // slope
const double B = 2;  // y-intercept
const double SIGMA = 1;  // stddev
const unsigned NUM_DATA_SAMPLES = 1000;

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
	        std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<cont_t> m_samples(NUM_SAMPLES), 
                                b_samples(NUM_SAMPLES);

            ppl::Param<cont_t>  m {m_samples.data()},
                                b {b_samples.data()};

	/*
            ppl::Data<cont_t> y_0 {cont_t(-3.028326723463461)};

            ppl::Data<cont_t> y_1 {cont_t(-2.933682208105621)};

            ppl::Data<cont_t> y_32 {cont_t(-1.182838468998551)};

            ppl::Data<cont_t> y_65 {cont_t(2.5003731118269448)};
		*/

            ppl::Data<cont_t> y_99 {cont_t(5.543896637640236)};

            auto model = (
                m |= ppl::uniform(cont_t(0), cont_t(10)),
                b |= ppl::uniform(cont_t(0), cont_t(10)),
                /*y_0 |= ppl::normal(m * cont_t(-10.0) + b, (SIGMA)),
                y_1 |= ppl::normal(m * cont_t(-9.798) + b, (SIGMA)),
              

                y_32 |= ppl::normal(m * cont_t(-3.535) + b, (SIGMA)),
           
                y_65 |= ppl::normal(m * cont_t(3.131) + b, (SIGMA)),*/
        
                y_99 |= ppl::normal(m * cont_t(10.0) + b, (SIGMA))
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
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
