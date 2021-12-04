#pragma once
#include <cassert>
#include <random>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/expression/variable/variable_viewer.hpp>
#include <autoppl/expression/distribution/uniform.hpp>
#include <autoppl/math/math.hpp>
#include <type_traits>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>


using namespace ppl::math;

namespace ppl {
namespace expr {




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	2 COMPONENT CONTINUOUS MIXTURE
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <typename p1_type, typename Dist1_type, typename Dist2_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else

#endif


struct Mixture : util::DistExpr<Mixture<p1_type, Dist1_type, Dist2_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    /*static_assert(
	       ((std::is_base_of<util::continuous_dist_base,Dist1_type>::value)
	    && (std::is_base_of<util::continuous_dist_base,Dist2_type>::value))
	||
	    ((std::is_base_of<util::discrete_dist_base,Dist1_type>::value)
	    && (std::is_base_of<util::discrete_dist_base,Dist2_type>::value))
	);*/

#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Mixture<p1_type,Dist1_type, Dist2_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    Mixture(p1_type p1, Dist1_type Dist1, Dist2_type Dist2)
        : p1_{p1}, Dist1_{Dist1}, Dist2_{Dist2}
    {}

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_.get_value()){
		return Dist1_.sample();
	}
	else {
		return Dist2_.sample();
	}
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_.get_value()){
		return Dist1_.sample();
	}
	else {
		return Dist2_.sample();
	}
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
  	return (p1_.get_value()*Dist1_.pdf(x)) + ((one_-p1_.get_value())*Dist2_.pdf(x));
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const 
    {
      return ppl::math::log( (p1_.get_value()*Dist1_.pdf(x)) + ((one_-p1_.get_value())*Dist2_.pdf(x))); 
    }

    value_t min() const { return (Dist1_.max() < Dist2_.max())? Dist1_.max() : Dist2_.max() ; }
    value_t max() const { return (Dist1_.max() < Dist2_.max())? Dist2_.max() : Dist1_.max() ; }


    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p1_type p1_;
    value_t one_ = value_t(1.);
    Dist1_type Dist1_;
    Dist2_type Dist2_;

};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	2 COMPONENT DISCRETE MIXTURE
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <typename p1_type, typename Dist1_type, typename Dist2_type, typename value_t_=util::disc_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
//template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct DiscreteMixture : util::DistExpr<DiscreteMixture<p1_type, Dist1_type, Dist2_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    /*static_assert(
	       ((std::is_base_of<util::continuous_dist_base,Dist1_type>::value)
	    && (std::is_base_of<util::continuous_dist_base,Dist2_type>::value))
	||
	    ((std::is_base_of<util::discrete_dist_base,Dist1_type>::value)
	    && (std::is_base_of<util::discrete_dist_base,Dist2_type>::value))
	);*/

#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<DiscreteMixture<p1_type,Dist1_type, Dist2_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    DiscreteMixture(p1_type p1, Dist1_type Dist1, Dist2_type Dist2)
        : p1_{p1}, Dist1_{Dist1}, Dist2_{Dist2}
    {}

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_.get_value()){
		return Dist1_.sample();
	}
	else {
		return Dist2_.sample();
	}
        //std::normal_distribution dist(mean(), stddev());
        //return dist(gen);
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_.get_value()){
		return Dist1_.sample();
	}
	else {
		return Dist2_.sample();
	}
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
  	return (p1_.get_value()*Dist1_.pdf(x)) + ((one_-p1_.get_value())*Dist2_.pdf(x));
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const 
    {
      return ppl::math::log( (p1_.get_value()*Dist1_.pdf(x)) + ((one_-p1_.get_value())*Dist2_.pdf(x))); //ppl::math::log( )
    }

    //value_t mean(size_t index=0) const { return mean_.get_value(index);}
    //value_t stddev(size_t index=0) const { return stddev_.get_value(index);}
    value_t min() const { return (Dist1_.max() < Dist2_.max())? Dist1_.max() : Dist2_.max() ; }
    value_t max() const { return (Dist1_.max() < Dist2_.max())? Dist2_.max() : Dist1_.max() ; }


    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p1_type p1_;
    value_t one_ = value_t(1.);
    Dist1_type Dist1_;
    Dist2_type Dist2_;

};




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  3 COMPONENT CONTINUOUS MIXTURE OF 3 DISTRIBUTIONS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <typename p1_type, typename Dist1_type,typename p2_type, typename Dist2_type,typename p3_type, typename Dist3_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
//template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct Mixture3 : util::DistExpr<Mixture3<p1_type,Dist1_type,p2_type, Dist2_type,p3_type, Dist3_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    //asserts all distributions are same type
    static_assert(std::is_base_of<util::continuous_dist_base,Dist1_type>::value);
    static_assert(std::is_base_of<util::continuous_dist_base,Dist2_type>::value);
    static_assert(std::is_base_of<util::continuous_dist_base,Dist3_type>::value);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Mixture3<p1_type,Dist1_type,p2_type, Dist2_type,p3_type, Dist3_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    Mixture3(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
        : p1_{p1}, p2_{p2}, p3_{p3}, Dist1_{Dist1}, Dist2_{Dist2}, Dist3_{Dist3}
    {}

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else {
		return Dist3_.sample();
	}
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else {
		return Dist3_.sample();
	}
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
  	return (p1_*Dist1_.pdf(x)) + (p2_*Dist2_.pdf(x))  + (p3_*Dist3_.pdf(x));
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const 
    {
      return ppl::math::log((p1_.get_value()*Dist1_.pdf(x)) + (p2_.get_value()*Dist2_.pdf(x)) + (p3_.get_value()*Dist3_.pdf(x))); 
    }

    value_t min() const {if (Dist1_.min() < Dist2_.min()){ return (Dist1_.min() < Dist3_.min())? Dist1_.min() : Dist3_.min() ;}
			else { return (Dist2_.min() < Dist3_.min())? Dist2_.min() : Dist3_.min() ;}}

    value_t max() const {if (Dist1_.max() < Dist2_.max()){ return (Dist2_.max() < Dist3_.max())? Dist3_.max() : Dist2_.max() ;}
			else { return (Dist1_.max() < Dist3_.max())? Dist3_.max() : Dist1_.max() ;}}



    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p1_type p1_;
    p2_type p2_;
    p3_type p3_;

    Dist1_type Dist1_;
    Dist2_type Dist2_;
    Dist3_type Dist3_;

};




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DISCRETE MIXTURE OF 3 DISTRIBUTIONS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <typename p1_type, typename Dist1_type,typename p2_type, typename Dist2_type,typename p3_type, typename Dist3_type, typename value_t_=util::disc_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
//template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct DiscreteMixture3 : util::DistExpr<DiscreteMixture3<p1_type,Dist1_type,p2_type, Dist2_type,p3_type, Dist3_type>, dist_value_t_>, util::discrete_dist_base
{

#if __cplusplus <= 201703L
    //asserts all distributions are same type
    static_assert(std::is_base_of<util::discrete_dist_base,Dist1_type>::value);
    static_assert(std::is_base_of<util::discrete_dist_base,Dist2_type>::value);
    static_assert(std::is_base_of<util::discrete_dist_base,Dist3_type>::value);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<DiscreteMixture3<p1_type,Dist1_type, p2_type, Dist2_type,p3_type, Dist3_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    DiscreteMixture3(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
        : p1_{p1}, p2_{p2}, p3_{p3}, Dist1_{Dist1}, Dist2_{Dist2}, Dist3_{Dist3}
    {}

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else {
		return Dist3_.sample();
	}
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else {
		return Dist3_.sample();
	}
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
  	return (p1_.get_value()*Dist1_.pdf(x)) + (p2_.get_value()*Dist2_.pdf(x))  + (p3_.get_value()*Dist3_.pdf(x));
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const 
    {
      return ppl::math::log((p1_.get_value()*Dist1_.pdf(x)) + (p2_.get_value()*Dist2_.pdf(x)) + (p3_.get_value()*Dist3_.pdf(x))); 
    }

    value_t min() const {if (Dist1_.min() < Dist2_.min()){ return (Dist1_.min() < Dist3_.min())? Dist1_.min() : Dist3_.min() ;}
			else { return (Dist2_.min() < Dist3_.min())? Dist2_.min() : Dist3_.min() ;}}

    value_t max() const {if (Dist1_.max() < Dist2_.max()){ return (Dist2_.max() < Dist3_.max())? Dist3_.max() : Dist2_.max() ;}
			else { return (Dist1_.max() < Dist3_.max())? Dist3_.max() : Dist1_.max() ;}}



    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p1_type p1_;
    p2_type p2_;
    p3_type p3_;

    Dist1_type Dist1_;
    Dist2_type Dist2_;
    Dist3_type Dist3_;

};






//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  4 COMPONENT CONTINUOUS MIXTURE OF 4 DISTRIBUTIONS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <typename p1_type, typename Dist1_type,typename p2_type, typename Dist2_type,typename p3_type, typename Dist3_type,typename p4_type, typename Dist4_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
//template <typename mean_type, typename stddev_type, typename value_t_=util::cont_param_t, typename dist_value_t_=util::dist_value_t_default>
#else
//template <util::var_expr mean_type, util::var_expr stddev_type> // NOT CHANGED
#endif


struct Mixture4 : util::DistExpr<Mixture4<p1_type,Dist1_type,p2_type, Dist2_type,p3_type, Dist3_type,p4_type, Dist4_type>, dist_value_t_>, util::continuous_dist_base
{

#if __cplusplus <= 201703L
    //asserts all distributions are same type
    static_assert(std::is_base_of<util::continuous_dist_base,Dist1_type>::value);
    static_assert(std::is_base_of<util::continuous_dist_base,Dist2_type>::value);
    static_assert(std::is_base_of<util::continuous_dist_base,Dist3_type>::value);
    static_assert(std::is_base_of<util::continuous_dist_base,Dist4_type>::value);
#endif

    using value_t = value_t_;
    using base_t = util::DistExpr<Mixture3<p1_type,Dist1_type,p2_type, Dist2_type,p3_type, Dist3_type,p4_type, Dist4_type>, dist_value_t_>;
    using dist_value_t = typename base_t::dist_value_t;
    using base_t::pdf;
    using base_t::log_pdf;


    Mixture4(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3,  p4_type p4, Dist4_type Dist4)
        : p1_{p1}, p2_{p2}, p3_{p3}, p4_{p4}, Dist1_{Dist1}, Dist2_{Dist2}, Dist3_{Dist3},Dist4_{Dist4}
    {}

    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<std::is_floating_point_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else if (a < p1_+p2_+p3_){
		return Dist3_.sample();
	}
	else {
		return Dist4_.sample();
	}
    }


    //our 32bit base type fixed point version
    template <class GeneratorType, class value_t_dummy = value_t>
    value_t sample(GeneratorType& gen, std::enable_if_t<ppl::is_fixed_32_v<value_t_dummy>>* = nullptr) const
    {
	value_t a = standard_uniform_primitive<value_t>(gen);
	if (a < p1_){
		return Dist1_.sample();
	}
	else if (a < p1_+p2_){
		return Dist2_.sample();
	}
	else if (a < p1_+p2_+p3_){
		return Dist3_.sample();
	}
	else {
		return Dist4_.sample();
	}
    }

    dist_value_t pdf(value_t x, size_t index=0) const 
    {
  	return (p1_*Dist1_.pdf(x)) + (p2_*Dist2_.pdf(x))  + (p4_*Dist4_.pdf(x))  +  (p3_*Dist3_.pdf(x));
    }

    dist_value_t log_pdf(value_t x, size_t index=0) const 
    {
      return ppl::math::log((p1_.get_value()*Dist1_.pdf(x)) + (p2_.get_value()*Dist2_.pdf(x)) + (p4_.get_value()*Dist4_.pdf(x)) + (p3_.get_value()*Dist3_.pdf(x))); 
    }

    value_t min() const {if (Dist1_.min() < Dist2_.min()){ return (Dist1_.min() < Dist3_.min())? Dist1_.min() : Dist3_.min() ;}
			else { return (Dist2_.min() < Dist3_.min())? Dist2_.min() : Dist3_.min() ;}}

    value_t max() const {if (Dist1_.max() < Dist2_.max()){ return (Dist2_.max() < Dist3_.max())? Dist3_.max() : Dist2_.max() ;}
			else { return (Dist1_.max() < Dist3_.max())? Dist3_.max() : Dist1_.max() ;}}



    std::set<boost::uuids::uuid> getDeps(){

    }


   DeterministicInterval getInterval(){

   }


   DeterministicInterval log_pdf_interval(DeterministicInterval& dataInterval, DeterministicInterval& biggestInterval){
	
   }

private:
    p1_type p1_;
    p2_type p2_;
    p3_type p3_;
    p4_type p4_;

    Dist1_type Dist1_;
    Dist2_type Dist2_;
    Dist3_type Dist3_;
    Dist4_type Dist4_;

};

} // namespace expr
} // namespace ppl
