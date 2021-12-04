#pragma once
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>

namespace ppl {
namespace expr {

struct ConstantBaseType {};

template <class ValueType>
struct Constant : util::VarExpr<Constant<ValueType>>, ConstantBaseType
{
    using value_t = ValueType;
    Constant(value_t c)
        : c_{c}
    {}
    value_t get_value(size_t = 0) const {
        return c_;
    }

    constexpr size_t size() const { return 1; }


    std::set<boost::uuids::uuid> getDeps(){
	std::set<boost::uuids::uuid> used_vars; 
	return used_vars;
    }
 
   //returns the trivial interval: [c,c]
   DeterministicInterval getInterval(){
	DeterministicInterval res = DeterministicInterval(double(c_),double(c_));
	return res;
   }


private:
    value_t c_;
};

} // namespace expr
} // namespace ppl
