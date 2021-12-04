#pragma once
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/variable.hpp>
#include <autoppl/math/math.hpp>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>
#include <autoppl/global_data.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <class BinaryOp, class LHSVarExprType, class RHSVarExprType>
#else
template <class BinaryOp, util::var_expr LHSVarExprType, util::var_expr RHSVarExprType>
#endif
struct BinaryOpNode : 
    util::VarExpr<BinaryOpNode<BinaryOp, LHSVarExprType, RHSVarExprType>>
{
#if __cplusplus <= 201703L
	static_assert(util::assert_is_var_expr_v<LHSVarExprType>);
	static_assert(util::assert_is_var_expr_v<RHSVarExprType>);
#endif

	using value_t = std::common_type_t<
		typename util::var_expr_traits<LHSVarExprType>::value_t,
		typename util::var_expr_traits<RHSVarExprType>::value_t
			>;

	BinaryOpNode(const LHSVarExprType& lhs, const RHSVarExprType& rhs)
		: lhs_{lhs}, rhs_{rhs}
	{ assert(lhs.size() == rhs.size() || lhs.size() == 1 || rhs.size() == 1); }

    value_t get_value(size_t i = 0) const {
        auto lhs_value = lhs_.get_value(i);
        auto rhs_value = rhs_.get_value(i);
        return BinaryOp::evaluate(lhs_value, rhs_value);
    }

    size_t size() const { return std::max(lhs_.size(), rhs_.size()); }

    /**
     * Returns ad expression of the binary operation.
     */
    template <class VecRefType, class VecADVarType>
    auto get_ad(const VecRefType& keys,
                const VecADVarType& vars,
                size_t idx = 0) const
    {  
        return BinaryOp::evaluate(lhs_.get_ad(keys, vars, idx),
                                  rhs_.get_ad(keys, vars, idx));
    }


    std::set<boost::uuids::uuid> getDeps(){
	std::set<boost::uuids::uuid> used_vars; 
	std::set<boost::uuids::uuid> lhs_used_vars = lhs_.getDeps(); //runtime polymorphism: whatever class mean_ is should have it's own getDeps() function
	used_vars.insert(lhs_used_vars.begin(),lhs_used_vars.end());
	std::set<boost::uuids::uuid> rhs_used_vars = rhs_.getDeps();
	used_vars.insert(rhs_used_vars.begin(),rhs_used_vars.end());
	return used_vars;
    }


   DeterministicInterval getInterval(){
        return BinaryOp::getInterval(lhs_, rhs_);
   }

private:
	LHSVarExprType lhs_;
	RHSVarExprType rhs_;

};

struct AddOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return x + y;
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return add(x.getInterval(),y.getInterval());
	}


};

struct SubOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return x - y;
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return sub(x.getInterval(),y.getInterval());
	}



};

struct MultOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return x * y;
	}


	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return mult(x.getInterval(),y.getInterval());
	}


};

struct DivOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return x / y;
	}

};

struct MaxOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x >= y) ? x : y;
	}


	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return max(x.getInterval(),y.getInterval());
	}

};

struct MinOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x <= y) ? x : y;
	}


	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return min(x.getInterval(),y.getInterval());
	}


};



struct GTEOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x >= y) ? LHSValueType(1.) : LHSValueType(0.);
	}


	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return DeterministicInterval(0.,1.);
	}



};


struct LTEOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x <= y) ? LHSValueType(1.) : LHSValueType(0.);
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return DeterministicInterval(0.,1.);
	}


};


struct GTOp {

	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x > y) ? LHSValueType(1.) : LHSValueType(0.);
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return DeterministicInterval(0.,1.);
	}


};



struct LTOp {

	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x < y) ? LHSValueType(1.) : LHSValueType(0.);
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return DeterministicInterval(0,1);
	}


};

struct EQOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x == y) ? LHSValueType(1.) : LHSValueType(0.);
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return DeterministicInterval(0,1);
	}

};


struct SqrtOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return ppl::math::sqrt(x*y);
	}

	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return sqrt_prod(x.getInterval(),y.getInterval());
	}


};


struct BooleanSwitchOp {
	//left value is a boolean switch
	template <class BooleanSwitchType, class RHSValueType>
	static auto evaluate(BooleanSwitchType x, RHSValueType y)
	{
		return x*y;
	}

	template <class BooleanSwitchType, class RHSValueType>
	static auto getInterval(BooleanSwitchType x, RHSValueType y)
	{
		return y.getInterval();
	}

};



struct MixtureMaxOp {
	
	template <class LHSValueType, class RHSValueType>
	static auto evaluate(LHSValueType x, RHSValueType y)
	{
		return (x >= y) ? x : y;
	}


	template <class LHSValueType, class RHSValueType>
	static auto getInterval(LHSValueType x, RHSValueType y)
	{
		return union_(x.getInterval(),y.getInterval());
	}

};



} // namespace expr
} // namespace ppl
