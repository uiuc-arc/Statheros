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
    std::cout << "command_relay,health_relay,status_relay,health_sensor,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;
    
    std::vector<Fixed>  command_relay_means(NUM_ITERS), 
                        health_relay_means(NUM_ITERS),
                        status_relay_means(NUM_ITERS),
                        health_sensor_means(NUM_ITERS);

    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

		std::vector<Fixed>  command_relay_samples(NUM_SAMPLES),
				health_relay_samples(NUM_SAMPLES),
				status_relay_samples(NUM_SAMPLES),
				health_sensor_samples(NUM_SAMPLES);

		ppl::Param<Fixed> command_relay {command_relay_samples.data()};
		ppl::Param<Fixed> health_relay {health_relay_samples.data()};
		ppl::Param<Fixed> status_relay {status_relay_samples.data()};
		ppl::Param<Fixed> health_sensor {health_sensor_samples.data()};

		Fixed one_(1.);
  		ppl::Data<Fixed> feedback_sensor {Fixed(0.)};

            auto model = (
		command_relay |= ppl::bernoulli(Fixed(0.5)),
		//health_relay |= ppl::discrete_mixture(Fixed(0.9), ppl::delta(Fixed(0.)),  Fixed(0.05), ppl::delta(Fixed(1.)),   Fixed(0.05), ppl::delta(Fixed(2.))),
		health_relay |= ppl::categorical(Fixed(0.05), Fixed(0.05), Fixed(0.9)),

		status_relay |= ppl::bernoulli( (Fixed(1.)-command_relay)*(health_relay==Fixed(2.))*Fixed(1.) +
						(Fixed(1.)-command_relay)*(health_relay==Fixed(1.))*Fixed(0.) +
							(Fixed(1.)-command_relay)*(health_relay==Fixed(0.))*Fixed(1.) +
					command_relay*(health_relay==Fixed(2.))*Fixed(0.) +
						command_relay*(health_relay==Fixed(1.))*Fixed(0.) +
							command_relay*(health_relay==Fixed(0.))*Fixed(1.)
					   ),

		//health_sensor |= ppl::discrete_mixture3(Fixed(0.9), ppl::delta(Fixed(0.)),  Fixed(0.05), ppl::delta(Fixed(1.)),   Fixed(0.05), ppl::delta(Fixed(2.))),
		health_sensor |= ppl::categorical(Fixed(0.05), Fixed(0.05), Fixed(0.9)),

		feedback_sensor |= ppl::bernoulli( status_relay*(health_sensor==Fixed(2.))*Fixed(1.) +
						status_relay*(health_sensor==Fixed(1.))*Fixed(0.) +
							status_relay*(health_sensor==Fixed(0.))*Fixed(1.) +
					(Fixed(1.)-status_relay)*(health_sensor==Fixed(2.))*Fixed(0.) +
						(Fixed(1.)-status_relay)*(health_sensor==Fixed(1.))*Fixed(0.) +
							(Fixed(1.)-status_relay)*(health_sensor==Fixed(0.))*Fixed(1.)
					   )
            );

 
            int burn_in_samples = static_cast<int>(NUM_SAMPLES * BURN_IN_PERCENTAGE / 100.);

            std::chrono::microseconds t_before_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());
            ppl::mh(model, NUM_SAMPLES, burn_in_samples);
            std::chrono::microseconds t_after_mh = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            auto acceptance_ratio = static_cast<double>(globals::num_accepts) / globals::num_samples;

            auto command_relay_mean = ppl::math::mean(command_relay_samples);
            auto health_relay_mean = ppl::math::mean(health_relay_samples);
            auto status_relay_mean = ppl::math::mean(status_relay_samples);
            auto health_sensor_mean = ppl::math::mean(health_sensor_samples);
        

            std::chrono::microseconds t_end = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            command_relay_means[i] = command_relay_mean;
            health_relay_means[i] = health_relay_mean;
            status_relay_means[i] = status_relay_mean;
            health_sensor_means[i] = health_sensor_mean;

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
        std::cout << command_relay_means[i] << ","
                << health_relay_means[i] << ","
                << status_relay_means[i] << ","
                << health_sensor_means[i] << ","
                << acceptance_ratios[i] << ","
                << t_ends[i].count() - t_starts[i].count() << ","
                << t_starts[i].count() << ","
                << t_ends[i].count() << ","
                << t_before_mhs[i].count() << ","
                << t_after_mhs[i].count() << std::endl;
    }
}
