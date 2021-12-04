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

const double M = 0.1;  // slope
const double B = 0.3;  // y-intercept
const double SIGMA = 0.02;  // stddev
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

            std::vector<cont_t>  m_samples(NUM_SAMPLES), 
                                b_samples(NUM_SAMPLES);

            ppl::Param<cont_t>   m {m_samples.data()},
                                b {b_samples.data()};

            ppl::Data<cont_t> y_0 {cont_t(-0.7083149641991614)};
            ppl::Data<cont_t> y_1 {cont_t(-0.6611550513919604)};
            ppl::Data<cont_t> y_2 {cont_t(-0.6608425425055817)};
            ppl::Data<cont_t> y_3 {cont_t(-0.6379842667670347)};
            ppl::Data<cont_t> y_4 {cont_t(-0.6148993520467548)};
            ppl::Data<cont_t> y_5 {cont_t(-0.5701703355566026)};
            ppl::Data<cont_t> y_6 {cont_t(-0.5910312317134895)};
            ppl::Data<cont_t> y_7 {cont_t(-0.553341010990055)};
            ppl::Data<cont_t> y_8 {cont_t(-0.5314525388608432)};
            ppl::Data<cont_t> y_9 {cont_t(-0.5110294105777234)};
            ppl::Data<cont_t> y_10 {cont_t(-0.47820327608562097)};
            ppl::Data<cont_t> y_11 {cont_t(-0.4581639219270807)};
            ppl::Data<cont_t> y_12 {cont_t(-0.4339584481382188)};
            ppl::Data<cont_t> y_13 {cont_t(-0.413827328404564)};
            ppl::Data<cont_t> y_14 {cont_t(-0.45960043333777756)};
            ppl::Data<cont_t> y_15 {cont_t(-0.3805767579790647)};
            ppl::Data<cont_t> y_16 {cont_t(-0.3757855926984384)};
            ppl::Data<cont_t> y_17 {cont_t(-0.35304612459437634)};
            ppl::Data<cont_t> y_18 {cont_t(-0.36655876910408514)};
            ppl::Data<cont_t> y_19 {cont_t(-0.3236466925629509)};
            ppl::Data<cont_t> y_20 {cont_t(-0.3081810540674223)};
            ppl::Data<cont_t> y_21 {cont_t(-0.26304188127972683)};
            ppl::Data<cont_t> y_22 {cont_t(-0.2163002887845351)};
            ppl::Data<cont_t> y_23 {cont_t(-0.23466171010157838)};
            ppl::Data<cont_t> y_24 {cont_t(-0.22242104018557649)};
            ppl::Data<cont_t> y_25 {cont_t(-0.1898888351362901)};
            ppl::Data<cont_t> y_26 {cont_t(-0.16486950382184495)};
            ppl::Data<cont_t> y_27 {cont_t(-0.1608112004603406)};
            ppl::Data<cont_t> y_28 {cont_t(-0.1871050410023992)};
            ppl::Data<cont_t> y_29 {cont_t(-0.10744634990196988)};
            ppl::Data<cont_t> y_30 {cont_t(-0.08047412555295817)};
            ppl::Data<cont_t> y_31 {cont_t(-0.06778574082828023)};
            ppl::Data<cont_t> y_32 {cont_t(-0.04188393546807097)};
            ppl::Data<cont_t> y_33 {cont_t(-0.06465463669132919)};
            ppl::Data<cont_t> y_34 {cont_t(-0.02551811434544558)};
            ppl::Data<cont_t> y_35 {cont_t(0.002802206409260424)};
            ppl::Data<cont_t> y_36 {cont_t(0.025236235738856264)};
            ppl::Data<cont_t> y_37 {cont_t(0.015937300963579107)};
            ppl::Data<cont_t> y_38 {cont_t(0.03471310102624979)};
            ppl::Data<cont_t> y_39 {cont_t(0.11335828672081714)};
            ppl::Data<cont_t> y_40 {cont_t(0.14236377794693583)};
            ppl::Data<cont_t> y_41 {cont_t(0.13093242818133888)};
            ppl::Data<cont_t> y_42 {cont_t(0.1271220521835167)};
            ppl::Data<cont_t> y_43 {cont_t(0.17994950023561343)};
            ppl::Data<cont_t> y_44 {cont_t(0.17216810982271222)};
            ppl::Data<cont_t> y_45 {cont_t(0.2015544050403495)};
            ppl::Data<cont_t> y_46 {cont_t(0.21110329189018417)};
            ppl::Data<cont_t> y_47 {cont_t(0.2545291373328213)};
            ppl::Data<cont_t> y_48 {cont_t(0.2815564284400493)};
            ppl::Data<cont_t> y_49 {cont_t(0.29047518544044565)};
            ppl::Data<cont_t> y_50 {cont_t(0.32895971704350957)};
            ppl::Data<cont_t> y_51 {cont_t(0.32590159686993286)};
            ppl::Data<cont_t> y_52 {cont_t(0.32470839867099904)};
            ppl::Data<cont_t> y_53 {cont_t(0.3790358251217788)};
            ppl::Data<cont_t> y_54 {cont_t(0.38518466810683116)};
            ppl::Data<cont_t> y_55 {cont_t(0.3867886804403706)};
            ppl::Data<cont_t> y_56 {cont_t(0.47269761069748645)};
            ppl::Data<cont_t> y_57 {cont_t(0.44124675270634317)};
            ppl::Data<cont_t> y_58 {cont_t(0.4885249828644222)};
            ppl::Data<cont_t> y_59 {cont_t(0.5040625767094881)};
            ppl::Data<cont_t> y_60 {cont_t(0.5224817057343638)};
            ppl::Data<cont_t> y_61 {cont_t(0.5484253074818038)};
            ppl::Data<cont_t> y_62 {cont_t(0.5488003788986606)};
            ppl::Data<cont_t> y_63 {cont_t(0.5605712026168925)};
            ppl::Data<cont_t> y_64 {cont_t(0.6223442918897882)};
            ppl::Data<cont_t> y_65 {cont_t(0.6460986131875823)};
            ppl::Data<cont_t> y_66 {cont_t(0.6201857343931416)};
            ppl::Data<cont_t> y_67 {cont_t(0.6579400370724959)};
            ppl::Data<cont_t> y_68 {cont_t(0.6849608275943361)};
            ppl::Data<cont_t> y_69 {cont_t(0.67880289088756)};
            ppl::Data<cont_t> y_70 {cont_t(0.7102274239429134)};
            ppl::Data<cont_t> y_71 {cont_t(0.7335357396812835)};
            ppl::Data<cont_t> y_72 {cont_t(0.7403089440316628)};
            ppl::Data<cont_t> y_73 {cont_t(0.7886672873711056)};
            ppl::Data<cont_t> y_74 {cont_t(0.7742846827157631)};
            ppl::Data<cont_t> y_75 {cont_t(0.8449215262758812)};
            ppl::Data<cont_t> y_76 {cont_t(0.8292011039557795)};
            ppl::Data<cont_t> y_77 {cont_t(0.8618934945333933)};
            ppl::Data<cont_t> y_78 {cont_t(0.8789835478337608)};
            ppl::Data<cont_t> y_79 {cont_t(0.9363257164437588)};
            ppl::Data<cont_t> y_80 {cont_t(0.9263428226588133)};
            ppl::Data<cont_t> y_81 {cont_t(0.9366470636196983)};
            ppl::Data<cont_t> y_82 {cont_t(0.934465207209848)};
            ppl::Data<cont_t> y_83 {cont_t(1.007421201072163)};
            ppl::Data<cont_t> y_84 {cont_t(1.0187310623963834)};
            ppl::Data<cont_t> y_85 {cont_t(1.044393827318987)};
            ppl::Data<cont_t> y_86 {cont_t(1.016115319044585)};
            ppl::Data<cont_t> y_87 {cont_t(1.0645969050113575)};
            ppl::Data<cont_t> y_88 {cont_t(1.0745810190320544)};
            ppl::Data<cont_t> y_89 {cont_t(1.0855968452119829)};
            ppl::Data<cont_t> y_90 {cont_t(1.1316885838113868)};
            ppl::Data<cont_t> y_91 {cont_t(1.1237585269083905)};
            ppl::Data<cont_t> y_92 {cont_t(1.1523500410997483)};
            ppl::Data<cont_t> y_93 {cont_t(1.2020980919240325)};
            ppl::Data<cont_t> y_94 {cont_t(1.2060570136552782)};
            ppl::Data<cont_t> y_95 {cont_t(1.2576288358972993)};
            ppl::Data<cont_t> y_96 {cont_t(1.2689062750531923)};
            ppl::Data<cont_t> y_97 {cont_t(1.2443826239201259)};
            ppl::Data<cont_t> y_98 {cont_t(1.257915804505404)};
            ppl::Data<cont_t> y_99 {cont_t(1.3003251183176385)};

            auto model = (
                m |= ppl::uniform(cont_t(0), cont_t(10)),
                b |= ppl::uniform(cont_t(0), cont_t(10)),
                y_0 |= ppl::normal(m * cont_t(-10.0) + b, cont_t(SIGMA)),
                y_1 |= ppl::normal(m * cont_t(-9.798) + b, cont_t(SIGMA)),
                y_2 |= ppl::normal(m * cont_t(-9.596) + b, cont_t(SIGMA)),
                y_3 |= ppl::normal(m * cont_t(-9.394) + b, cont_t(SIGMA)),
                y_4 |= ppl::normal(m * cont_t(-9.192) + b, cont_t(SIGMA)),
                y_5 |= ppl::normal(m * cont_t(-8.99) + b, cont_t(SIGMA)),
                y_6 |= ppl::normal(m * cont_t(-8.788) + b, cont_t(SIGMA)),
                y_7 |= ppl::normal(m * cont_t(-8.586) + b, cont_t(SIGMA)),
                y_8 |= ppl::normal(m * cont_t(-8.384) + b, cont_t(SIGMA)),
                y_9 |= ppl::normal(m * cont_t(-8.182) + b, cont_t(SIGMA)),
                y_10 |= ppl::normal(m * cont_t(-7.98) + b, cont_t(SIGMA)),
                y_11 |= ppl::normal(m * cont_t(-7.778) + b, cont_t(SIGMA)),
                y_12 |= ppl::normal(m * cont_t(-7.576) + b, cont_t(SIGMA)),
                y_13 |= ppl::normal(m * cont_t(-7.374) + b, cont_t(SIGMA)),
                y_14 |= ppl::normal(m * cont_t(-7.172) + b, cont_t(SIGMA)),
                y_15 |= ppl::normal(m * cont_t(-6.97) + b, cont_t(SIGMA)),
                y_16 |= ppl::normal(m * cont_t(-6.768) + b, cont_t(SIGMA)),
                y_17 |= ppl::normal(m * cont_t(-6.566) + b, cont_t(SIGMA)),
                y_18 |= ppl::normal(m * cont_t(-6.364) + b, cont_t(SIGMA)),
                y_19 |= ppl::normal(m * cont_t(-6.162) + b, cont_t(SIGMA)),
                y_20 |= ppl::normal(m * cont_t(-5.96) + b, cont_t(SIGMA)),
                y_21 |= ppl::normal(m * cont_t(-5.758) + b, cont_t(SIGMA)),
                y_22 |= ppl::normal(m * cont_t(-5.556) + b, cont_t(SIGMA)),
                y_23 |= ppl::normal(m * cont_t(-5.354) + b, cont_t(SIGMA)),
                y_24 |= ppl::normal(m * cont_t(-5.152) + b, cont_t(SIGMA)),
                y_25 |= ppl::normal(m * cont_t(-4.949) + b, cont_t(SIGMA)),
                y_26 |= ppl::normal(m * cont_t(-4.747) + b, cont_t(SIGMA)),
                y_27 |= ppl::normal(m * cont_t(-4.545) + b, cont_t(SIGMA)),
                y_28 |= ppl::normal(m * cont_t(-4.343) + b, cont_t(SIGMA)),
                y_29 |= ppl::normal(m * cont_t(-4.141) + b, cont_t(SIGMA)),
                y_30 |= ppl::normal(m * cont_t(-3.939) + b, cont_t(SIGMA)),
                y_31 |= ppl::normal(m * cont_t(-3.737) + b, cont_t(SIGMA)),
                y_32 |= ppl::normal(m * cont_t(-3.535) + b, cont_t(SIGMA)),
                y_33 |= ppl::normal(m * cont_t(-3.333) + b, cont_t(SIGMA)),
                y_34 |= ppl::normal(m * cont_t(-3.131) + b, cont_t(SIGMA)),
                y_35 |= ppl::normal(m * cont_t(-2.929) + b, cont_t(SIGMA)),
                y_36 |= ppl::normal(m * cont_t(-2.727) + b, cont_t(SIGMA)),
                y_37 |= ppl::normal(m * cont_t(-2.525) + b, cont_t(SIGMA)),
                y_38 |= ppl::normal(m * cont_t(-2.323) + b, cont_t(SIGMA)),
                y_39 |= ppl::normal(m * cont_t(-2.121) + b, cont_t(SIGMA)),
                y_40 |= ppl::normal(m * cont_t(-1.919) + b, cont_t(SIGMA)),
                y_41 |= ppl::normal(m * cont_t(-1.717) + b, cont_t(SIGMA)),
                y_42 |= ppl::normal(m * cont_t(-1.515) + b, cont_t(SIGMA)),
                y_43 |= ppl::normal(m * cont_t(-1.313) + b, cont_t(SIGMA)),
                y_44 |= ppl::normal(m * cont_t(-1.111) + b, cont_t(SIGMA)),
                y_45 |= ppl::normal(m * cont_t(-0.909) + b, cont_t(SIGMA)),
                y_46 |= ppl::normal(m * cont_t(-0.707) + b, cont_t(SIGMA)),
                y_47 |= ppl::normal(m * cont_t(-0.505) + b, cont_t(SIGMA)),
                y_48 |= ppl::normal(m * cont_t(-0.303) + b, cont_t(SIGMA)),
                y_49 |= ppl::normal(m * cont_t(-0.101) + b, cont_t(SIGMA)),
                y_50 |= ppl::normal(m * cont_t(0.101) + b, cont_t(SIGMA)),
                y_51 |= ppl::normal(m * cont_t(0.303) + b, cont_t(SIGMA)),
                y_52 |= ppl::normal(m * cont_t(0.505) + b, cont_t(SIGMA)),
                y_53 |= ppl::normal(m * cont_t(0.707) + b, cont_t(SIGMA)),
                y_54 |= ppl::normal(m * cont_t(0.909) + b, cont_t(SIGMA)),
                y_55 |= ppl::normal(m * cont_t(1.111) + b, cont_t(SIGMA)),
                y_56 |= ppl::normal(m * cont_t(1.313) + b, cont_t(SIGMA)),
                y_57 |= ppl::normal(m * cont_t(1.515) + b, cont_t(SIGMA)),
                y_58 |= ppl::normal(m * cont_t(1.717) + b, cont_t(SIGMA)),
                y_59 |= ppl::normal(m * cont_t(1.919) + b, cont_t(SIGMA)),
                y_60 |= ppl::normal(m * cont_t(2.121) + b, cont_t(SIGMA)),
                y_61 |= ppl::normal(m * cont_t(2.323) + b, cont_t(SIGMA)),
                y_62 |= ppl::normal(m * cont_t(2.525) + b, cont_t(SIGMA)),
                y_63 |= ppl::normal(m * cont_t(2.727) + b, cont_t(SIGMA)),
                y_64 |= ppl::normal(m * cont_t(2.929) + b, cont_t(SIGMA)),
                y_65 |= ppl::normal(m * cont_t(3.131) + b, cont_t(SIGMA)),
                y_66 |= ppl::normal(m * cont_t(3.333) + b, cont_t(SIGMA)),
                y_67 |= ppl::normal(m * cont_t(3.535) + b, cont_t(SIGMA)),
                y_68 |= ppl::normal(m * cont_t(3.737) + b, cont_t(SIGMA)),
                y_69 |= ppl::normal(m * cont_t(3.939) + b, cont_t(SIGMA)),
                y_70 |= ppl::normal(m * cont_t(4.141) + b, cont_t(SIGMA)),
                y_71 |= ppl::normal(m * cont_t(4.343) + b, cont_t(SIGMA)),
                y_72 |= ppl::normal(m * cont_t(4.545) + b, cont_t(SIGMA)),
                y_73 |= ppl::normal(m * cont_t(4.747) + b, cont_t(SIGMA)),
                y_74 |= ppl::normal(m * cont_t(4.949) + b, cont_t(SIGMA)),
                y_75 |= ppl::normal(m * cont_t(5.152) + b, cont_t(SIGMA)),
                y_76 |= ppl::normal(m * cont_t(5.354) + b, cont_t(SIGMA)),
                y_77 |= ppl::normal(m * cont_t(5.556) + b, cont_t(SIGMA)),
                y_78 |= ppl::normal(m * cont_t(5.758) + b, cont_t(SIGMA)),
                y_79 |= ppl::normal(m * cont_t(5.96) + b, cont_t(SIGMA)),
                y_80 |= ppl::normal(m * cont_t(6.162) + b, cont_t(SIGMA)),
                y_81 |= ppl::normal(m * cont_t(6.364) + b, cont_t(SIGMA)),
                y_82 |= ppl::normal(m * cont_t(6.566) + b, cont_t(SIGMA)),
                y_83 |= ppl::normal(m * cont_t(6.768) + b, cont_t(SIGMA)),
                y_84 |= ppl::normal(m * cont_t(6.97) + b, cont_t(SIGMA)),
                y_85 |= ppl::normal(m * cont_t(7.172) + b, cont_t(SIGMA)),
                y_86 |= ppl::normal(m * cont_t(7.374) + b, cont_t(SIGMA)),
                y_87 |= ppl::normal(m * cont_t(7.576) + b, cont_t(SIGMA)),
                y_88 |= ppl::normal(m * cont_t(7.778) + b, cont_t(SIGMA)),
                y_89 |= ppl::normal(m * cont_t(7.98) + b, cont_t(SIGMA)),
                y_90 |= ppl::normal(m * cont_t(8.182) + b, cont_t(SIGMA)),
                y_91 |= ppl::normal(m * cont_t(8.384) + b, cont_t(SIGMA)),
                y_92 |= ppl::normal(m * cont_t(8.586) + b, cont_t(SIGMA)),
                y_93 |= ppl::normal(m * cont_t(8.788) + b, cont_t(SIGMA)),
                y_94 |= ppl::normal(m * cont_t(8.99) + b, cont_t(SIGMA)),
                y_95 |= ppl::normal(m * cont_t(9.192) + b, cont_t(SIGMA)),
                y_96 |= ppl::normal(m * cont_t(9.394) + b, cont_t(SIGMA)),
                y_97 |= ppl::normal(m * cont_t(9.596) + b, cont_t(SIGMA)),
                y_98 |= ppl::normal(m * cont_t(9.798) + b, cont_t(SIGMA)),
                y_99 |= ppl::normal(m * cont_t(10.0) + b, cont_t(SIGMA))
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
