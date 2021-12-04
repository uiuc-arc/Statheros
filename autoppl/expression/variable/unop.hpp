#pragma once
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/variable.hpp>
#include <autoppl/math/math.hpp>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ppl {
namespace expr {

#if __cplusplus <= 201703L
template <class UnaryOp, class LHSVarExprType>
#else
template <class UnaryOp, util::var_expr LHSVarExprType>
#endif
struct UnaryOpNode : 
    util::VarExpr<UnaryOpNode<UnaryOp, LHSVarExprType> >
{
#if __cplusplus <= 201703L
	static_assert(util::assert_is_var_expr_v<LHSVarExprType>);

#endif

	using value_t = std::common_type_t<
		typename util::var_expr_traits<LHSVarExprType>::value_t>;

	UnaryOpNode(const LHSVarExprType& lhs)
		: lhs_{lhs}
	{ assert(true); }

    value_t get_value(size_t i = 0) const {
        auto lhs_value = lhs_.get_value(i);
        return UnaryOp::evaluate(lhs_value);
    }

    size_t size() const { return lhs_.size(); }

    std::set<boost::uuids::uuid> getDeps(){
	std::set<boost::uuids::uuid> used_vars; 
	std::set<boost::uuids::uuid> lhs_used_vars = lhs_.getDeps(); //runtime polymorphism: whatever class mean_ is should have it's own getDeps() function
	return lhs_used_vars;
    }




private:
	LHSVarExprType lhs_;

};

struct StepOp {
	
	template <class LHSValueType>
	static auto evaluate(LHSValueType x)
	{
		LHSValueType zero_(0.);
		LHSValueType one_(1.); 
		return x > zero_ ? one_ : zero_ ;
	}

};


} // namespace expr
} // namespace ppl
