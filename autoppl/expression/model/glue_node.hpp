#pragma once
#include <type_traits>
#include <functional>
#include <autoppl/util/model_expr_traits.hpp>
#include <iostream>
#include <typeinfo>       // operator typeid
#include <autoppl/program_analysis/dependencies_intervals.hpp>
#include <set>

namespace ppl {
namespace expr {

class GlueNodeParent {};

/**
 * This class represents a "node" in a model expression that
 * "glues" two sub-model expressions.
 */
#if __cplusplus <= 201703L
template <class LHSNodeType, class RHSNodeType>
#else
template <util::model_expr LHSNodeType, util::model_expr RHSNodeType>
#endif
struct GlueNode : util::ModelExpr<GlueNode<LHSNodeType, RHSNodeType>>, GlueNodeParent
{

#if __cplusplus <= 201703L
    static_assert(util::assert_is_model_expr_v<LHSNodeType>);
    static_assert(util::assert_is_model_expr_v<RHSNodeType>);
#endif

    using left_node_t = LHSNodeType;
    using right_node_t = RHSNodeType;
    using dist_value_t = std::common_type_t<
        typename util::model_expr_traits<left_node_t>::dist_value_t,
        typename util::model_expr_traits<right_node_t>::dist_value_t
            >;

    GlueNode(const left_node_t& lhs,
             const right_node_t& rhs) noexcept
        : left_node_{lhs}
        , right_node_{rhs}
    {}

    /**
     * Generic traversal function.
     * Recursively traverses left then right.
     */
    template <class EqNodeFunc>
    void traverse(EqNodeFunc&& eq_f)
    {
        left_node_.traverse(eq_f);
        right_node_.traverse(eq_f);
    }

    template <class EqNodeFunc>
    void traverse(EqNodeFunc&& eq_f) const
    {
        left_node_.traverse(eq_f);
        right_node_.traverse(eq_f);
    }

    /**
     * Computes left node joint pdf then right node joint pdf
     * and returns the product of the two.
     */
    dist_value_t pdf() const
    { return left_node_.pdf() * right_node_.pdf(); }

    /**
     * Computes left node joint log-pdf then right node joint log-pdf
     * and returns the sum of the two.
     */
    dist_value_t log_pdf() const
    { return left_node_.log_pdf() + right_node_.log_pdf(); }


     /*void printName() {
	std::cout << "I'm a glue node" << std::endl;
        left_node_.printName();
	right_node_.printName();
     }*/


    /**
     * Up to constant addition, returns ad expression of log pdf
     * of both sides added together.
     */
    template <class VecRefType, class VecADVarType>
    auto ad_log_pdf(const VecRefType& keys,
                    const VecADVarType& vars) const
    {
        return (left_node_.ad_log_pdf(keys, vars) +
                right_node_.ad_log_pdf(keys, vars));
    }


    std::set<boost::uuids::uuid> getDeps(){
	return right_node_.getDeps();
    }

    left_node_t getLeftChild(){
	return left_node_;
    }

    right_node_t getRightChild(){
	return right_node_;
    }


    void getInterval(){
	left_node_.getInterval();
	right_node_.getInterval();
    }

    void getLikelihoodInterval(){
	left_node_.getLikelihoodInterval();
	right_node_.getLikelihoodInterval();
    }


private:
    left_node_t left_node_;
    right_node_t right_node_;
};

} // namespace expr
} // namespace ppl
