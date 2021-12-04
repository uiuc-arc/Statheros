#pragma once
#include <algorithm>
#include <type_traits>
#include <functional>
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/model_expr_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <type_traits>
#include <typeinfo>       // operator typeid
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <autoppl/program_analysis/dependencies_intervals.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>

namespace ppl {
namespace expr {

struct EqNodeParent {};
struct EqNodeCont {};
struct EqNodeDisc {};

/**
 * This class represents a "node" in the model expression
 * that relates a var with a distribution.
 */
#if __cplusplus <= 201703L
template <class VarType, class DistType>
#else
template <util::var VarType, util::dist_expr DistType>
#endif
struct EqNode : util::ModelExpr<EqNode<VarType, DistType>>, EqNodeParent, std::conditional_t<std::is_base_of_v<util::continuous_dist_base, DistType>, EqNodeCont, EqNodeDisc>
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_v<VarType>);
    static_assert(util::assert_is_dist_expr_v<DistType>);
#endif

    using var_t = VarType;
    using dist_t = DistType;
    using dist_value_t = typename util::dist_expr_traits<dist_t>::dist_value_t;

    EqNode(var_t& var, 
           const dist_t& dist) noexcept
        : orig_var_ref_{var}
        , dist_{dist}
    {}


    template <class EqNodeFunc>
    void traverse(EqNodeFunc&& eq_f)
    {
        using this_t = EqNode<VarType, DistType>;
        eq_f(static_cast<this_t&>(*this));
    }

    template <class EqNodeFunc>
    void traverse(EqNodeFunc&& eq_f) const
    {
        using this_t = EqNode<VarType, DistType>;
        eq_f(static_cast<const this_t&>(*this));
    }


    dist_value_t pdf() const {
        return dist_.pdf(get_variable());
    }

    dist_value_t log_pdf() const {
        return dist_.log_pdf(get_variable());
    }


    auto& get_variable() { return orig_var_ref_.get(); }
    const auto& get_variable() const { return orig_var_ref_.get(); }
    const auto& get_distribution() const { return dist_; }


    std::set<boost::uuids::uuid> getDeps(){
	return dist_.getDeps();
    }

    std::set<boost::uuids::uuid> getVarID(){
	return orig_var_ref_.get().getDeps();
    }


    void getInterval(){
	//get  distribution's interval
	DeterministicInterval dist_range = dist_.getInterval();
	//get  var's id
	boost::uuids::uuid var_id = orig_var_ref_.get().getUUID();
	std::cout << dist_range.upper << std::endl;
	//map them together
	globals::intervals.insert({var_id,dist_range});
    }

    void getLikelihoodInterval(){

	//check if my variable is a data or param
	if constexpr (util::is_data_v<var_t>){
		boost::uuids::uuid var_id = orig_var_ref_.get().getUUID();
		int NumDataPoints = orig_var_ref_.get().size();
		DeterministicInterval DataInterval = orig_var_ref_.get().getDataInterval();
		
		DeterministicInterval BiggestInterval(0.,0.);
		DeterministicInterval LikelihoodInterval = dist_.log_pdf_interval(DataInterval,BiggestInterval);

		globals::single_likelihood_intervals.insert({var_id,LikelihoodInterval});

		//account for the likelihood sum over all data points
		if (NumDataPoints > 0){
			LikelihoodInterval = scale(double(NumDataPoints)/double(4),LikelihoodInterval);
		}
		BiggestInterval = union_(BiggestInterval,LikelihoodInterval);

		globals::likelihood_intervals.insert({var_id,LikelihoodInterval});
		globals::biggest_intervals.insert({var_id,BiggestInterval});
	}

    }


private:
    using var_ref_t = std::reference_wrapper<var_t>;    
    var_ref_t orig_var_ref_;                           
    dist_t dist_;                 


};

} // namespace expr
} // namespace ppl
