#include <chrono>
#include <ctime>   
#include <iostream>
#include <random>
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

const double M = 0.1;  // slope
const double B = 0.3;  // y-intercept
const double SIGMA = 0.02;  // stddev
const unsigned NUM_DATA_SAMPLES = 1000;

int main() {
    std::cout << "m,b,acceptance_ratio,time_elapsed_us,start_time,end_time,mh_start_time,mh_end_time" << std::endl;

    std::vector<Fixed> m_means(NUM_ITERS), b_means(NUM_ITERS);
    std::vector<double> acceptance_ratios(NUM_ITERS);
    std::vector<microseconds>   t_starts(NUM_ITERS),
                                t_ends(NUM_ITERS),
                                t_before_mhs(NUM_ITERS),
                                t_after_mhs(NUM_ITERS);

    for (int i = 0; i < NUM_ITERS; ++i) {
        try {
            std::chrono::microseconds t_start = duration_cast<microseconds>(std::chrono::system_clock::now().time_since_epoch());

            std::vector<Fixed>  m_samples(NUM_SAMPLES), 
                                b_samples(NUM_SAMPLES);

            ppl::Param<Fixed>   m {m_samples.data()},
                                b {b_samples.data()};

            ppl::Data<Fixed> y_0 {Fixed(-0.7083149641991614)};
            ppl::Data<Fixed> y_1 {Fixed(-0.6611550513919604)};
            ppl::Data<Fixed> y_2 {Fixed(-0.6608425425055817)};
            ppl::Data<Fixed> y_3 {Fixed(-0.6379842667670347)};
            ppl::Data<Fixed> y_4 {Fixed(-0.6148993520467548)};
            ppl::Data<Fixed> y_5 {Fixed(-0.5701703355566026)};
            ppl::Data<Fixed> y_6 {Fixed(-0.5910312317134895)};
            ppl::Data<Fixed> y_7 {Fixed(-0.553341010990055)};
            ppl::Data<Fixed> y_8 {Fixed(-0.5314525388608432)};
            ppl::Data<Fixed> y_9 {Fixed(-0.5110294105777234)};
            ppl::Data<Fixed> y_10 {Fixed(-0.47820327608562097)};
            ppl::Data<Fixed> y_11 {Fixed(-0.4581639219270807)};
            ppl::Data<Fixed> y_12 {Fixed(-0.4339584481382188)};
            ppl::Data<Fixed> y_13 {Fixed(-0.413827328404564)};
            ppl::Data<Fixed> y_14 {Fixed(-0.45960043333777756)};
            ppl::Data<Fixed> y_15 {Fixed(-0.3805767579790647)};
            ppl::Data<Fixed> y_16 {Fixed(-0.3757855926984384)};
            ppl::Data<Fixed> y_17 {Fixed(-0.35304612459437634)};
            ppl::Data<Fixed> y_18 {Fixed(-0.36655876910408514)};
            ppl::Data<Fixed> y_19 {Fixed(-0.3236466925629509)};
            ppl::Data<Fixed> y_20 {Fixed(-0.3081810540674223)};
            ppl::Data<Fixed> y_21 {Fixed(-0.26304188127972683)};
            ppl::Data<Fixed> y_22 {Fixed(-0.2163002887845351)};
            ppl::Data<Fixed> y_23 {Fixed(-0.23466171010157838)};
            ppl::Data<Fixed> y_24 {Fixed(-0.22242104018557649)};
            ppl::Data<Fixed> y_25 {Fixed(-0.1898888351362901)};
            ppl::Data<Fixed> y_26 {Fixed(-0.16486950382184495)};
            ppl::Data<Fixed> y_27 {Fixed(-0.1608112004603406)};
            ppl::Data<Fixed> y_28 {Fixed(-0.1871050410023992)};
            ppl::Data<Fixed> y_29 {Fixed(-0.10744634990196988)};
            ppl::Data<Fixed> y_30 {Fixed(-0.08047412555295817)};
            ppl::Data<Fixed> y_31 {Fixed(-0.06778574082828023)};
            ppl::Data<Fixed> y_32 {Fixed(-0.04188393546807097)};
            ppl::Data<Fixed> y_33 {Fixed(-0.06465463669132919)};
            ppl::Data<Fixed> y_34 {Fixed(-0.02551811434544558)};
            ppl::Data<Fixed> y_35 {Fixed(0.002802206409260424)};
            ppl::Data<Fixed> y_36 {Fixed(0.025236235738856264)};
            ppl::Data<Fixed> y_37 {Fixed(0.015937300963579107)};
            ppl::Data<Fixed> y_38 {Fixed(0.03471310102624979)};
            ppl::Data<Fixed> y_39 {Fixed(0.11335828672081714)};
            ppl::Data<Fixed> y_40 {Fixed(0.14236377794693583)};
            ppl::Data<Fixed> y_41 {Fixed(0.13093242818133888)};
            ppl::Data<Fixed> y_42 {Fixed(0.1271220521835167)};
            ppl::Data<Fixed> y_43 {Fixed(0.17994950023561343)};
            ppl::Data<Fixed> y_44 {Fixed(0.17216810982271222)};
            ppl::Data<Fixed> y_45 {Fixed(0.2015544050403495)};
            ppl::Data<Fixed> y_46 {Fixed(0.21110329189018417)};
            ppl::Data<Fixed> y_47 {Fixed(0.2545291373328213)};
            ppl::Data<Fixed> y_48 {Fixed(0.2815564284400493)};
            ppl::Data<Fixed> y_49 {Fixed(0.29047518544044565)};
            ppl::Data<Fixed> y_50 {Fixed(0.32895971704350957)};
            ppl::Data<Fixed> y_51 {Fixed(0.32590159686993286)};
            ppl::Data<Fixed> y_52 {Fixed(0.32470839867099904)};
            ppl::Data<Fixed> y_53 {Fixed(0.3790358251217788)};
            ppl::Data<Fixed> y_54 {Fixed(0.38518466810683116)};
            ppl::Data<Fixed> y_55 {Fixed(0.3867886804403706)};
            ppl::Data<Fixed> y_56 {Fixed(0.47269761069748645)};
            ppl::Data<Fixed> y_57 {Fixed(0.44124675270634317)};
            ppl::Data<Fixed> y_58 {Fixed(0.4885249828644222)};
            ppl::Data<Fixed> y_59 {Fixed(0.5040625767094881)};
            ppl::Data<Fixed> y_60 {Fixed(0.5224817057343638)};
            ppl::Data<Fixed> y_61 {Fixed(0.5484253074818038)};
            ppl::Data<Fixed> y_62 {Fixed(0.5488003788986606)};
            ppl::Data<Fixed> y_63 {Fixed(0.5605712026168925)};
            ppl::Data<Fixed> y_64 {Fixed(0.6223442918897882)};
            ppl::Data<Fixed> y_65 {Fixed(0.6460986131875823)};
            ppl::Data<Fixed> y_66 {Fixed(0.6201857343931416)};
            ppl::Data<Fixed> y_67 {Fixed(0.6579400370724959)};
            ppl::Data<Fixed> y_68 {Fixed(0.6849608275943361)};
            ppl::Data<Fixed> y_69 {Fixed(0.67880289088756)};
            ppl::Data<Fixed> y_70 {Fixed(0.7102274239429134)};
            ppl::Data<Fixed> y_71 {Fixed(0.7335357396812835)};
            ppl::Data<Fixed> y_72 {Fixed(0.7403089440316628)};
            ppl::Data<Fixed> y_73 {Fixed(0.7886672873711056)};
            ppl::Data<Fixed> y_74 {Fixed(0.7742846827157631)};
            ppl::Data<Fixed> y_75 {Fixed(0.8449215262758812)};
            ppl::Data<Fixed> y_76 {Fixed(0.8292011039557795)};
            ppl::Data<Fixed> y_77 {Fixed(0.8618934945333933)};
            ppl::Data<Fixed> y_78 {Fixed(0.8789835478337608)};
            ppl::Data<Fixed> y_79 {Fixed(0.9363257164437588)};
            ppl::Data<Fixed> y_80 {Fixed(0.9263428226588133)};
            ppl::Data<Fixed> y_81 {Fixed(0.9366470636196983)};
            ppl::Data<Fixed> y_82 {Fixed(0.934465207209848)};
            ppl::Data<Fixed> y_83 {Fixed(1.007421201072163)};
            ppl::Data<Fixed> y_84 {Fixed(1.0187310623963834)};
            ppl::Data<Fixed> y_85 {Fixed(1.044393827318987)};
            ppl::Data<Fixed> y_86 {Fixed(1.016115319044585)};
            ppl::Data<Fixed> y_87 {Fixed(1.0645969050113575)};
            ppl::Data<Fixed> y_88 {Fixed(1.0745810190320544)};
            ppl::Data<Fixed> y_89 {Fixed(1.0855968452119829)};
            ppl::Data<Fixed> y_90 {Fixed(1.1316885838113868)};
            ppl::Data<Fixed> y_91 {Fixed(1.1237585269083905)};
            ppl::Data<Fixed> y_92 {Fixed(1.1523500410997483)};
            ppl::Data<Fixed> y_93 {Fixed(1.2020980919240325)};
            ppl::Data<Fixed> y_94 {Fixed(1.2060570136552782)};
            ppl::Data<Fixed> y_95 {Fixed(1.2576288358972993)};
            ppl::Data<Fixed> y_96 {Fixed(1.2689062750531923)};
            ppl::Data<Fixed> y_97 {Fixed(1.2443826239201259)};
            ppl::Data<Fixed> y_98 {Fixed(1.257915804505404)};
            ppl::Data<Fixed> y_99 {Fixed(1.3003251183176385)};

            auto model = (
                m |= ppl::uniform(Fixed(0), Fixed(10)),
                b |= ppl::uniform(Fixed(0), Fixed(10)),
                y_0 |= ppl::normal(m * Fixed(-10.0) + b, Fixed(SIGMA)),
                y_1 |= ppl::normal(m * Fixed(-9.798) + b, Fixed(SIGMA)),
                y_2 |= ppl::normal(m * Fixed(-9.596) + b, Fixed(SIGMA)),
                y_3 |= ppl::normal(m * Fixed(-9.394) + b, Fixed(SIGMA)),
                y_4 |= ppl::normal(m * Fixed(-9.192) + b, Fixed(SIGMA)),
                y_5 |= ppl::normal(m * Fixed(-8.99) + b, Fixed(SIGMA)),
                y_6 |= ppl::normal(m * Fixed(-8.788) + b, Fixed(SIGMA)),
                y_7 |= ppl::normal(m * Fixed(-8.586) + b, Fixed(SIGMA)),
                y_8 |= ppl::normal(m * Fixed(-8.384) + b, Fixed(SIGMA)),
                y_9 |= ppl::normal(m * Fixed(-8.182) + b, Fixed(SIGMA)),
                y_10 |= ppl::normal(m * Fixed(-7.98) + b, Fixed(SIGMA)),
                y_11 |= ppl::normal(m * Fixed(-7.778) + b, Fixed(SIGMA)),
                y_12 |= ppl::normal(m * Fixed(-7.576) + b, Fixed(SIGMA)),
                y_13 |= ppl::normal(m * Fixed(-7.374) + b, Fixed(SIGMA)),
                y_14 |= ppl::normal(m * Fixed(-7.172) + b, Fixed(SIGMA)),
                y_15 |= ppl::normal(m * Fixed(-6.97) + b, Fixed(SIGMA)),
                y_16 |= ppl::normal(m * Fixed(-6.768) + b, Fixed(SIGMA)),
                y_17 |= ppl::normal(m * Fixed(-6.566) + b, Fixed(SIGMA)),
                y_18 |= ppl::normal(m * Fixed(-6.364) + b, Fixed(SIGMA)),
                y_19 |= ppl::normal(m * Fixed(-6.162) + b, Fixed(SIGMA)),
                y_20 |= ppl::normal(m * Fixed(-5.96) + b, Fixed(SIGMA)),
                y_21 |= ppl::normal(m * Fixed(-5.758) + b, Fixed(SIGMA)),
                y_22 |= ppl::normal(m * Fixed(-5.556) + b, Fixed(SIGMA)),
                y_23 |= ppl::normal(m * Fixed(-5.354) + b, Fixed(SIGMA)),
                y_24 |= ppl::normal(m * Fixed(-5.152) + b, Fixed(SIGMA)),
                y_25 |= ppl::normal(m * Fixed(-4.949) + b, Fixed(SIGMA)),
                y_26 |= ppl::normal(m * Fixed(-4.747) + b, Fixed(SIGMA)),
                y_27 |= ppl::normal(m * Fixed(-4.545) + b, Fixed(SIGMA)),
                y_28 |= ppl::normal(m * Fixed(-4.343) + b, Fixed(SIGMA)),
                y_29 |= ppl::normal(m * Fixed(-4.141) + b, Fixed(SIGMA)),
                y_30 |= ppl::normal(m * Fixed(-3.939) + b, Fixed(SIGMA)),
                y_31 |= ppl::normal(m * Fixed(-3.737) + b, Fixed(SIGMA)),
                y_32 |= ppl::normal(m * Fixed(-3.535) + b, Fixed(SIGMA)),
                y_33 |= ppl::normal(m * Fixed(-3.333) + b, Fixed(SIGMA)),
                y_34 |= ppl::normal(m * Fixed(-3.131) + b, Fixed(SIGMA)),
                y_35 |= ppl::normal(m * Fixed(-2.929) + b, Fixed(SIGMA)),
                y_36 |= ppl::normal(m * Fixed(-2.727) + b, Fixed(SIGMA)),
                y_37 |= ppl::normal(m * Fixed(-2.525) + b, Fixed(SIGMA)),
                y_38 |= ppl::normal(m * Fixed(-2.323) + b, Fixed(SIGMA)),
                y_39 |= ppl::normal(m * Fixed(-2.121) + b, Fixed(SIGMA)),
                y_40 |= ppl::normal(m * Fixed(-1.919) + b, Fixed(SIGMA)),
                y_41 |= ppl::normal(m * Fixed(-1.717) + b, Fixed(SIGMA)),
                y_42 |= ppl::normal(m * Fixed(-1.515) + b, Fixed(SIGMA)),
                y_43 |= ppl::normal(m * Fixed(-1.313) + b, Fixed(SIGMA)),
                y_44 |= ppl::normal(m * Fixed(-1.111) + b, Fixed(SIGMA)),
                y_45 |= ppl::normal(m * Fixed(-0.909) + b, Fixed(SIGMA)),
                y_46 |= ppl::normal(m * Fixed(-0.707) + b, Fixed(SIGMA)),
                y_47 |= ppl::normal(m * Fixed(-0.505) + b, Fixed(SIGMA)),
                y_48 |= ppl::normal(m * Fixed(-0.303) + b, Fixed(SIGMA)),
                y_49 |= ppl::normal(m * Fixed(-0.101) + b, Fixed(SIGMA)),
                y_50 |= ppl::normal(m * Fixed(0.101) + b, Fixed(SIGMA)),
                y_51 |= ppl::normal(m * Fixed(0.303) + b, Fixed(SIGMA)),
                y_52 |= ppl::normal(m * Fixed(0.505) + b, Fixed(SIGMA)),
                y_53 |= ppl::normal(m * Fixed(0.707) + b, Fixed(SIGMA)),
                y_54 |= ppl::normal(m * Fixed(0.909) + b, Fixed(SIGMA)),
                y_55 |= ppl::normal(m * Fixed(1.111) + b, Fixed(SIGMA)),
                y_56 |= ppl::normal(m * Fixed(1.313) + b, Fixed(SIGMA)),
                y_57 |= ppl::normal(m * Fixed(1.515) + b, Fixed(SIGMA)),
                y_58 |= ppl::normal(m * Fixed(1.717) + b, Fixed(SIGMA)),
                y_59 |= ppl::normal(m * Fixed(1.919) + b, Fixed(SIGMA)),
                y_60 |= ppl::normal(m * Fixed(2.121) + b, Fixed(SIGMA)),
                y_61 |= ppl::normal(m * Fixed(2.323) + b, Fixed(SIGMA)),
                y_62 |= ppl::normal(m * Fixed(2.525) + b, Fixed(SIGMA)),
                y_63 |= ppl::normal(m * Fixed(2.727) + b, Fixed(SIGMA)),
                y_64 |= ppl::normal(m * Fixed(2.929) + b, Fixed(SIGMA)),
                y_65 |= ppl::normal(m * Fixed(3.131) + b, Fixed(SIGMA)),
                y_66 |= ppl::normal(m * Fixed(3.333) + b, Fixed(SIGMA)),
                y_67 |= ppl::normal(m * Fixed(3.535) + b, Fixed(SIGMA)),
                y_68 |= ppl::normal(m * Fixed(3.737) + b, Fixed(SIGMA)),
                y_69 |= ppl::normal(m * Fixed(3.939) + b, Fixed(SIGMA)),
                y_70 |= ppl::normal(m * Fixed(4.141) + b, Fixed(SIGMA)),
                y_71 |= ppl::normal(m * Fixed(4.343) + b, Fixed(SIGMA)),
                y_72 |= ppl::normal(m * Fixed(4.545) + b, Fixed(SIGMA)),
                y_73 |= ppl::normal(m * Fixed(4.747) + b, Fixed(SIGMA)),
                y_74 |= ppl::normal(m * Fixed(4.949) + b, Fixed(SIGMA)),
                y_75 |= ppl::normal(m * Fixed(5.152) + b, Fixed(SIGMA)),
                y_76 |= ppl::normal(m * Fixed(5.354) + b, Fixed(SIGMA)),
                y_77 |= ppl::normal(m * Fixed(5.556) + b, Fixed(SIGMA)),
                y_78 |= ppl::normal(m * Fixed(5.758) + b, Fixed(SIGMA)),
                y_79 |= ppl::normal(m * Fixed(5.96) + b, Fixed(SIGMA)),
                y_80 |= ppl::normal(m * Fixed(6.162) + b, Fixed(SIGMA)),
                y_81 |= ppl::normal(m * Fixed(6.364) + b, Fixed(SIGMA)),
                y_82 |= ppl::normal(m * Fixed(6.566) + b, Fixed(SIGMA)),
                y_83 |= ppl::normal(m * Fixed(6.768) + b, Fixed(SIGMA)),
                y_84 |= ppl::normal(m * Fixed(6.97) + b, Fixed(SIGMA)),
                y_85 |= ppl::normal(m * Fixed(7.172) + b, Fixed(SIGMA)),
                y_86 |= ppl::normal(m * Fixed(7.374) + b, Fixed(SIGMA)),
                y_87 |= ppl::normal(m * Fixed(7.576) + b, Fixed(SIGMA)),
                y_88 |= ppl::normal(m * Fixed(7.778) + b, Fixed(SIGMA)),
                y_89 |= ppl::normal(m * Fixed(7.98) + b, Fixed(SIGMA)),
                y_90 |= ppl::normal(m * Fixed(8.182) + b, Fixed(SIGMA)),
                y_91 |= ppl::normal(m * Fixed(8.384) + b, Fixed(SIGMA)),
                y_92 |= ppl::normal(m * Fixed(8.586) + b, Fixed(SIGMA)),
                y_93 |= ppl::normal(m * Fixed(8.788) + b, Fixed(SIGMA)),
                y_94 |= ppl::normal(m * Fixed(8.99) + b, Fixed(SIGMA)),
                y_95 |= ppl::normal(m * Fixed(9.192) + b, Fixed(SIGMA)),
                y_96 |= ppl::normal(m * Fixed(9.394) + b, Fixed(SIGMA)),
                y_97 |= ppl::normal(m * Fixed(9.596) + b, Fixed(SIGMA)),
                y_98 |= ppl::normal(m * Fixed(9.798) + b, Fixed(SIGMA)),
                y_99 |= ppl::normal(m * Fixed(10.0) + b, Fixed(SIGMA))
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
