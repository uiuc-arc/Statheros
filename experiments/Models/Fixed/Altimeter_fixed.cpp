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
    std::cout << "H_L,U_A,H_B,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed>  H_L_means(NUM_ITERS), 
                        U_A_means(NUM_ITERS),
                        H_B_means(NUM_ITERS);

    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

		std::vector<Fixed>  H_L_samples(NUM_SAMPLES),
				U_A_samples(NUM_SAMPLES),
				H_B_samples(NUM_SAMPLES);

		ppl::Param<Fixed> H_L {H_L_samples.data()};
		ppl::Param<Fixed> U_A {U_A_samples.data()};
		ppl::Param<Fixed> H_B {H_B_samples.data()};

		Fixed one_(1.);
		ppl::Data<Fixed> S_L {one_};
		ppl::Data<Fixed> S_B {one_};
		ppl::Data<Fixed> S_S {one_};


            auto model = (
		    H_L |= ppl::bernoulli(Fixed(0.3)),
		    U_A |= ppl::bernoulli(Fixed(0.5)),
		    H_B |= ppl::bernoulli(Fixed(0.1)),

		    S_L |= ppl::bernoulli( ((one_-U_A)*(one_-H_L)*Fixed(0.)) +
		 			   ((one_-U_A)*(H_L)*Fixed(0.5)) +
		 			   ((U_A)*(one_-H_L)*Fixed(1.)) + 
		 			   ((U_A)*(H_L)*Fixed(0.5))
					),

		    S_B |= ppl::bernoulli( ((one_-U_A)*(one_-H_B)*Fixed(0.)) +
		 			   ((one_-U_A)*(H_B)*Fixed(0.5)) +
		 			   ((U_A)*(one_-H_B)*Fixed(1.)) + 
		 			   ((U_A)*(H_B)*Fixed(0.5))
					), 

		    S_S |= ppl::discrete_mixture((one_-U_A)*Fixed(0.7)+(U_A*Fixed(0.1)), ppl::delta(Fixed(0.)), 
								 (one_-U_A)*Fixed(0.1)+(U_A*Fixed(0.7)), ppl::delta(Fixed(1.)),
									 (one_-U_A)*Fixed(0.2)+(U_A*Fixed(0.2)), ppl::delta(Fixed(2.)))	
            );

 
            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto H_L_mean = ppl::math::mean(H_L_samples);
            auto U_A_mean = ppl::math::mean(U_A_samples);
            auto H_B_mean = ppl::math::mean(H_B_samples);
        
            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            H_L_means[i] = H_L_mean;
            U_A_means[i] = U_A_mean;
            H_B_means[i] = H_B_mean;

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
        std::cout << H_L_means[i] << ","
                << U_A_means[i] << ","
                << H_B_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
