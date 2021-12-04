#pragma once
#include <autoppl/util/traits.hpp>
#include <autoppl/expression/model/eq_node.hpp>
#include <autoppl/expression/model/glue_node.hpp>
#include <autoppl/expression/variable/variable_viewer.hpp>
#include <autoppl/expression/variable/constant.hpp>
#include <autoppl/expression/variable/binop.hpp>
#include <autoppl/expression/variable/unop.hpp>
#include <autoppl/variable.hpp>
#include <autoppl/expression/distribution/uniform.hpp>
#include <autoppl/expression/distribution/mixture.hpp>
#include <autoppl/expression/distribution/triangular.hpp>
#include <autoppl/expression/distribution/normal.hpp>
#include <autoppl/expression/distribution/bernoulli.hpp>
#include <autoppl/expression/distribution/categorical.hpp>
#include <autoppl/expression/distribution/delta.hpp>
#include <autoppl/expression/distribution/discrete_uniform.hpp>
#include <iostream>
#include <autoppl/settings.hpp>

namespace ppl {

////////////////////////////////////////////////////////
// Distribution Expression Builder
////////////////////////////////////////////////////////

namespace details {

/**
 * Converter from arbitrary (decayed) type to valid continuous parameter type 
 * by the following mapping:
 * - is_var_v<T> true => VariableViewer<T>
 * - T is an arithmetic type => Constant<T>
 * - is_var_expr_v<T> true => T
 * Assumes each condition is non-overlapping.
 */

#if __cplusplus <= 201703L

template <class T, class = void>
struct convert_to_param
{};

template <class T>
struct convert_to_param<T,
    std::enable_if_t<util::is_var_v<std::decay_t<T>> >
    >
{
    using type = expr::VariableViewer<std::decay_t<T>>;
};

template <class T>
struct convert_to_param<T, 
    std::enable_if_t<ppl::is_arithmetic_v<std::decay_t<T>> >
    >
{
    using type = expr::Constant<std::decay_t<T>>;
};

template <class T>
struct convert_to_param<T, 
    std::enable_if_t<util::is_var_expr_v<std::decay_t<T>> > 
    >
{
    using type = T;
};

#else

template <class T>
struct convert_to_param;

template <class T>
requires util::var<std::decay_t<T>>
struct convert_to_param<T>
{
    using type = expr::VariableViewer<std::decay_t<T>>;
};

template <class T>
requires ppl::is_arithmeticy_v<std::decay_t<T>>
struct convert_to_param<T>
{
    using type = expr::Constant<std::decay_t<T>>;
};

template <class T>
requires util::var_expr<std::decay_t<T>> 
struct convert_to_param<T> 
{
    using type = T;
};

#endif

template <class T>
using convert_to_param_t = 
    typename convert_to_param<T>::type;

#if __cplusplus <= 201703L

/**
 * Checks if valid distribution parameter:
 * - can be arithmetic
 * - if not arithmetic, must be variable or variable expression
 * - if variable, cannot be (rvalue reference or const)
 */
template <class T>
inline constexpr bool is_valid_dist_param_v =
    ppl::is_arithmetic_v<std::decay_t<T>> ||
    (util::is_var_v<std::decay_t<T>> && 
     !std::is_rvalue_reference_v<T> &&
     !std::is_const_v<std::remove_reference_t<T>>) ||
    (util::is_var_expr_v<std::decay_t<T>>)
    ;

/**
 * Checks if the decayed types of T1 and T2 
 * are not both arithmetic types.
 */
template <class T1, class T2>
inline constexpr bool is_not_both_arithmetic_v =
    !(ppl::is_arithmetic_v<std::decay_t<T1>> &&
      ppl::is_arithmetic_v<std::decay_t<T2>>)
    ;

#else

template <class T>
concept valid_dist_param =
    ppl::is_arithmetic_v<std::decay_t<T>> ||
    (util::var<std::decay_t<T>> &&
     !std::is_rvalue_reference_v<T> &&
     !std::is_const_v<std::remove_reference_t<T>>) ||
    (util::var_expr<std::decay_t<T>>)
    ;

template <class T1, class T2>
concept not_both_arithmetic =
    !(ppl::is_arithmetic_v<std::decay_t<T1>> &&
      ppl::is_arithmetic_v<std::decay_t<T2>>)
    ;

#endif

//////////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 201703L
template <class T1>
inline constexpr bool is_arithmetic_v =
    (ppl::is_arithmetic_v<std::decay_t<T1>>);

#else


template <class T1>
concept is_arithmetic =
    (ppl::is_arithmetic_v<std::decay_t<T1>>);

#endif
//////////////////////////////////////////////////////////////////////////////


} // namespace details

/**
 * Builds a Uniform expression only when the parameters
 * are both valid continuous distribution parameter types.
 * See var_expr.hpp for more information.
 */
#if __cplusplus <= 201703L
template <class MinType, class MaxType, class ValueT=util::cont_param_t, class DistValueT=util::dist_value_t_default
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > >
inline constexpr auto uniform(MinType&& min_expr,
                              MaxType&& max_expr)
#else
template <details::valid_dist_param MinType
        , details::valid_dist_param MaxType>
inline constexpr auto uniform(MinType&& min_expr,
                              MaxType&& max_expr)
#endif
{
    using min_t = details::convert_to_param_t<MinType>;
    using max_t = details::convert_to_param_t<MaxType>;

    min_t wrap_min_expr = std::forward<MinType>(min_expr);
    max_t wrap_max_expr = std::forward<MaxType>(max_expr);

    return expr::Uniform<min_t, max_t, ValueT, DistValueT>(wrap_min_expr, wrap_max_expr);
}

#if __cplusplus <= 201703L
template <class MinType, class MaxType, class ValueT=util::cont_param_t, class DistValueT=util::dist_value_t_default
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > >
inline constexpr auto discrete_uniform(MinType&& min_expr,
                                       MaxType&& max_expr)
#else
template <details::valid_dist_param MinType
        , details::valid_dist_param MaxType>
inline constexpr auto discrete_uniform(MinType&& min_expr,
                                       MaxType&& max_expr)
#endif
{
    using min_t = details::convert_to_param_t<MinType>;
    using max_t = details::convert_to_param_t<MaxType>;

    min_t wrap_min_expr = std::forward<MinType>(min_expr);
    max_t wrap_max_expr = std::forward<MaxType>(max_expr);

    return expr::DiscreteUniform<min_t, max_t, ValueT, DistValueT>(wrap_min_expr, wrap_max_expr);
}





/**
 * Builds a Triangular Distribution when parameters a, b, and c
 * are all valid continuous distribution parameter types.
 */
#if __cplusplus <= 201703L
template <class AType, class BType, class CType, class ValueT=util::cont_param_t, class DistValueT=util::dist_value_t_default
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<AType> &&
            details::is_valid_dist_param_v<BType>
         > >
inline constexpr auto triangular(AType&& a_expr,
                             BType&& b_expr, CType&& c_expr)
#else
template <details::valid_dist_param AType
        , details::valid_dist_param BType>
        , details::valid_dist_param CType>
inline constexpr auto triangular(AType&& a_expr,
                             BType&& b_expr, CType&& c_expr)
#endif
{
    using a_t = details::convert_to_param_t<AType>;
    using b_t = details::convert_to_param_t<BType>;
    using c_t = details::convert_to_param_t<CType>;

    a_t wrap_a_expr = std::forward<AType>(a_expr);
    b_t wrap_b_expr = std::forward<BType>(b_expr);
    c_t wrap_c_expr = std::forward<CType>(c_expr);

    return expr::Triangular<a_t, b_t, c_t, ValueT, DistValueT>(wrap_a_expr, wrap_b_expr, wrap_c_expr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	THE FOLLOWING ARE THE OVERLOADED VERSIONS OF MIXTURE DISTRIBUTION PRIMITIVES
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Builds a Mixture Distribution when parameters
 * are all valid continuous distribution parameter types.
 */
#if __cplusplus <= 201703L
template <class p1_type, class Dist1_type, class Dist2_type, class value_t_=util::value_t_bernoulli, class dist_value_t_=util::dist_value_t_default>
       /* , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > > */
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, Dist2_type Dist2)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class p1_type, class Dist1_type, class Dist2_type, class value_t_=util::value_t_bernoulli, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, Dist2_type Dist2)
#endif
{
    /*using min_t = details::convert_to_param_t<MinType>;
    using max_t = details::convert_to_param_t<MaxType>;

    min_t wrap_min_expr = std::forward<MinType>(min_expr);
    max_t wrap_max_expr = std::forward<MaxType>(max_expr);*/

    using b_type = details::convert_to_param_t<p1_type>;
    b_type wrap_b_expr = std::forward<b_type>(p1);

    /*using d1_t = details::convert_to_param_t<Dist1_type>;
    d1_t wrap_d1_expr = std::forward<d1_t>(Dist1);

    using d2_t = details::convert_to_param_t<Dist2_type>;
    d2_t wrap_d2_expr = std::forward<d2_t>(Dist2);*/

    return expr::Mixture<b_type, Dist1_type, Dist2_type>(wrap_b_expr, Dist1, Dist2);
}


/**
 * Builds a 3 component CONTINUOUS Mixture Distribution when parameters
 * are all valid continuous distribution parameter types.
 */
#if __cplusplus <= 201703L
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type, class value_t_=util::cont_param_t, class dist_value_t_=util::dist_value_t_default>
       /* , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > > */
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type, class value_t_=util::cont_param_t, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
#endif
{
    //wraps constants into an Expression, eg the type "double" gets wrapped into the type Constant (subtype of Expression)
    using p1_t = details::convert_to_param_t<p1_type>;
    p1_t wrap_p1_expr = std::forward<p1_type>(p1);

    using p2_t = details::convert_to_param_t<p2_type>;
    p2_t wrap_p2_expr = std::forward<p2_type>(p2);

    using p3_t = details::convert_to_param_t<p3_type>;
    p3_t wrap_p3_expr = std::forward<p3_type>(p3);

    return expr::Mixture3<p1_t,Dist1_type,p2_t, Dist2_type, p3_t, Dist3_type>(wrap_p1_expr, Dist1, wrap_p2_expr, Dist2, wrap_p3_expr, Dist3);
}

/**
 * Builds a 4 component CONTINUOUS Mixture Distribution when parameters
 * are all valid continuous distribution parameter types.
 */
#if __cplusplus <= 201703L
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type,class p4_type, class Dist4_type, class value_t_=util::cont_param_t, class dist_value_t_=util::dist_value_t_default>
       /* , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > > */
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3, p4_type p4, Dist4_type Dist4)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type,class p4_type, class Dist4_type, class value_t_=util::cont_param_t, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3, p4_type p4, Dist4_type Dist4)
#endif
{
    //wraps constants into an Expression, eg the type "double" gets wrapped into the type Constant (subtype of Expression)
    using p1_t = details::convert_to_param_t<p1_type>;
    p1_t wrap_p1_expr = std::forward<p1_type>(p1);

    using p2_t = details::convert_to_param_t<p2_type>;
    p2_t wrap_p2_expr = std::forward<p2_type>(p2);

    using p3_t = details::convert_to_param_t<p3_type>;
    p3_t wrap_p3_expr = std::forward<p3_type>(p3);

    using p4_t = details::convert_to_param_t<p4_type>;
    p4_t wrap_p4_expr = std::forward<p4_type>(p4);

    //return expr::Mixture3<p1_type,Dist1_type,p2_type, Dist2_type, p3_type, Dist3_type>(p1, Dist1, p2, Dist2, p3, Dist3);
    return expr::Mixture4<p1_t,Dist1_type,p2_t, Dist2_type, p3_t, Dist3_type,p4_t, Dist4_type>(wrap_p1_expr, Dist1, wrap_p2_expr, Dist2, wrap_p3_expr, Dist3, wrap_p4_expr, Dist4);
}


/**
 * Builds a 3 component DISCRETE Mixture Distribution when parameters
 * are all valid continuous distribution parameter types.
 */
#if __cplusplus <= 201703L
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type, class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>
       /* , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > > */
inline constexpr auto discrete_mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class p1_type, class Dist1_type, class p2_type, class Dist2_type,class p3_type, class Dist3_type, class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto discrete_mixture(p1_type p1, Dist1_type Dist1, p2_type p2, Dist2_type Dist2, p3_type p3, Dist3_type Dist3)
#endif
{

    //wraps constants into an Expression, eg the type "double" gets wrapped into the type Constant (subtype of Expression)
    using p1_t = details::convert_to_param_t<p1_type>;
    p1_t wrap_p1_expr = std::forward<p1_type>(p1);

    using p2_t = details::convert_to_param_t<p2_type>;
    p2_t wrap_p2_expr = std::forward<p2_type>(p2);

    using p3_t = details::convert_to_param_t<p3_type>;
    p3_t wrap_p3_expr = std::forward<p3_type>(p3);

    return expr::DiscreteMixture3<p1_t,Dist1_type,p2_t, Dist2_type, p3_t, Dist3_type>(wrap_p1_expr, Dist1, wrap_p2_expr, Dist2, wrap_p3_expr, Dist3);

}




/**
 * Builds a Normal expression only when the parameters
 * are both valid continuous distribution parameter types.
 * See var_expr.hpp for more information.
 */
#if __cplusplus <= 201703L
template <class MeanType, class StddevType, class ValueT=util::cont_param_t, class DistValueT=util::dist_value_t_default
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<MeanType> &&
            details::is_valid_dist_param_v<StddevType>
         > >
inline constexpr auto normal(MeanType&& mean_expr,
                             StddevType&& stddev_expr)
#else
template <details::valid_dist_param MeanType
        , details::valid_dist_param StddevType>
inline constexpr auto normal(MeanType&& mean_expr,
                             StddevType&& stddev_expr)
#endif
{
    using mean_t = details::convert_to_param_t<MeanType>;
    using stddev_t = details::convert_to_param_t<StddevType>;

    mean_t wrap_mean_expr = std::forward<MeanType>(mean_expr);
    stddev_t wrap_stddev_expr = std::forward<StddevType>(stddev_expr);

    return expr::Normal<mean_t, stddev_t, ValueT, DistValueT>(wrap_mean_expr, wrap_stddev_expr);
}

/**
 * Builds a Bernoulli expression only when the parameter
 * is a valid discrete distribution parameter type.
 * See var_expr.hpp for more information.
 */
#if __cplusplus <= 201703L
template <class ProbType, class ValueT=util::value_t_bernoulli, class DistValueT=util::dist_value_t_bernoulli
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<ProbType>
        > >
inline constexpr auto bernoulli(ProbType&& p_expr)
#else
template <details::valid_dist_param ProbType>
inline constexpr auto bernoulli(ProbType&& p_expr)
#endif
{
    using p_t = details::convert_to_param_t<ProbType>;
    p_t wrap_p_expr = std::forward<ProbType>(p_expr);
    return expr::Bernoulli<p_t, ValueT, DistValueT>(wrap_p_expr);
}


/**
 * Builds a Dirac Delta expression only when the parameters
 */
#if __cplusplus <= 201703L
template <class MeanType, class ValueT=util::cont_param_t, class DistValueT=util::dist_value_t_default
        , class = std::enable_if_t<
            details::is_valid_dist_param_v<MeanType> 
         > >
inline constexpr auto delta(MeanType&& mean_expr)
#else
template <details::valid_dist_param MeanType>
inline constexpr auto delta(MeanType&& mean_expr)
#endif
{
    using mean_t = details::convert_to_param_t<MeanType>;


    mean_t wrap_mean_expr = std::forward<MeanType>(mean_expr);


    return expr::Delta<mean_t, ValueT, DistValueT>(wrap_mean_expr);
}



/**
 * Builds a 3 component Categorical Distribution 
 */
#if __cplusplus <= 201703L
template <class p_type,class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>
       /* , class = std::enable_if_t<
            details::is_valid_dist_param_v<MinType> &&
            details::is_valid_dist_param_v<MaxType>
         > > */
inline constexpr auto categorical(p_type p1, p_type p2, p_type p3)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class p_type, class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto categorical(p_type p1, p_type p2, p_type p3)
#endif
{
    //wraps constants into an Expression, eg the type "double" gets wrapped into the type Constant (subtype of Expression)
    using p_t = details::convert_to_param_t<p_type>;
    p_t wrap_p1_expr = std::forward<p_t>(p1);

    //using p2_t = details::convert_to_param_t<p2_type>;
    p_t wrap_p2_expr = std::forward<p_t>(p2);

    //using p3_t = details::convert_to_param_t<p3_type>;
    p_t wrap_p3_expr = std::forward<p_t>(p3);

    //return expr::Mixture3<p1_type,Dist1_type,p2_type, Dist2_type, p3_type, Dist3_type>(p1, Dist1, p2, Dist2, p3, Dist3);
    return expr::Categorical<p_t,value_t_,dist_value_t_>(wrap_p1_expr, wrap_p2_expr, wrap_p3_expr);
}



#if __cplusplus <= 201703L
template <class bool_type, class Dist1_type, class Dist2_type,class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>

inline constexpr auto if_then_else(bool_type b, Dist1_type d1, Dist2_type d2)
#else
//template <details::valid_dist_param MinType
//        , details::valid_dist_param MaxType>
template <class bool_type, class Dist1_type, class Dist2_type,class value_t_=util::disc_param_t, class dist_value_t_=util::dist_value_t_default>
inline constexpr auto if_then_else(bool_type b, Dist1_type d1, Dist2_type d2)
#endif
{

    using b_type = details::convert_to_param_t<bool_type>;
    b_type wrap_b_expr = std::forward<b_type>(b);

    //using d1_t = details::convert_to_param_t<Dist1_type>;
    //d1_t wrap_d1_expr = std::forward<Dist1_type>(d1);

    //using d2_t = details::convert_to_param_t<Dist2_type>;
    //d2_t wrap_d2_expr = std::forward<Dist2_type>(d2);

    //return expr::Mixture3<p1_type,Dist1_type,p2_type, Dist2_type, p3_type, Dist3_type>(p1, Dist1, p2, Dist2, p3, Dist3);
    return expr::Mixture<b_type,Dist1_type,Dist2_type>(wrap_b_expr, d1, d2);
}





////////////////////////////////////////////////////////
// Model Expression Builder
////////////////////////////////////////////////////////

/**
 * Builds an EqNode to associate var with dist
 * only when var is a Variable and dist is a valid distribution expression.
 * Ex. x |= uniform(0,1)
 */
template <class VarType, class DistType>
inline constexpr auto operator|=(
        util::Var<VarType>& var,
        const util::DistExpr<DistType, typename DistType::dist_value_t>& dist) 
{ return expr::EqNode(var.self(), dist.self()); }

/**
 * Builds a GlueNode to "glue" the left expression with the right
 * only when both parameters are valid model expressions.
 * Ex. (x |= uniform(0,1), y |= uniform(0, 2))
 */
template <class LHSNodeType, class RHSNodeType>
inline constexpr auto operator,(const util::ModelExpr<LHSNodeType>& lhs,
                                const util::ModelExpr<RHSNodeType>& rhs)
{ return expr::GlueNode(lhs.self(), rhs.self()); }

////////////////////////////////////////////////////////
// Variable Expression Builder
////////////////////////////////////////////////////////

namespace details {

#if __cplusplus <= 201703L

/**
 * Concept of valid variable expression parameter
 * for the operator overloads:
 * - can be arithmetic type
 * - if not arithmetic, must be variable expression
 *   or a variable.
 */
template <class T>
inline constexpr bool is_valid_op_param_v =
    ppl::is_arithmetic_v<std::decay_t<T>> || 
    util::is_var_expr_v<std::decay_t<T>> ||
    util::is_var_v<std::decay_t<T>>
    ;
#else

template <class T>
concept valid_op_param =
    ppl::is_arithmetic_v<std::decay_t<T>> || 
    util::var_expr<std::decay_t<T>> ||
    util::var<std::decay_t<T>>
    ;

#endif

template <class Op, class LHSType, class RHSType>
inline constexpr auto operator_helper(LHSType&& lhs, RHSType&& rhs)
{
    // note: may be reference types if converted to itself
	using lhs_t = details::convert_to_param_t<LHSType>;
    using rhs_t = details::convert_to_param_t<RHSType>;

   	lhs_t wrap_lhs_expr = std::forward<LHSType>(lhs);
    rhs_t wrap_rhs_expr = std::forward<RHSType>(rhs);
    
    using binary_t = expr::BinaryOpNode<
        Op, std::decay_t<lhs_t>, std::decay_t<rhs_t>
    >;

    // lhs_t and rhs_t are decayed by node_t
	return binary_t(wrap_lhs_expr, wrap_rhs_expr);
}




template <class UnOp, class LHSType>
inline constexpr auto unary_operator_helper(LHSType&& lhs)
{
    // note: may be reference types if converted to itself
	using lhs_t = details::convert_to_param_t<LHSType>;


   	lhs_t wrap_lhs_expr = std::forward<LHSType>(lhs);

    
    using unary_t = expr::UnaryOpNode<
        UnOp, std::decay_t<lhs_t> >;

    // lhs_t and rhs_t are decayed by node_t
	return unary_t(wrap_lhs_expr);
}





} // namespace details

/**
 * Operator overloads, which only check for type-safety.
 * SFINAE to ensure concepts are placed.
 */

#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator+(LHSType&& lhs,
	                            RHSType&& rhs)
{ 
    return details::operator_helper<expr::AddOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}

#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator-(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::SubOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}

#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator*(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::MultOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}

#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator/(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::DivOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto max(LHSType&& lhs,
	                            RHSType&& rhs)
{ 
    return details::operator_helper<expr::MaxOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}

#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto min(LHSType&& lhs,
	                            RHSType&& rhs)
{ 
    return details::operator_helper<expr::MinOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



//heaviside step function used simulate conditional branching
#if __cplusplus <= 201703L
template <class LHSType
        , class = std::enable_if_t<
            //details::is_not_both_arithmetic_v<LHSType, LHSType> &&	//
            //details::is_arithmetic_v<LHSType> &&
            details::is_valid_op_param_v<LHSType>
        > >
#else
template <details::valid_op_param LHSType>
//requires details::not_both_arithmetic<LHSType>
#endif
inline constexpr auto step(LHSType&& lhs)
{ 
    return details::unary_operator_helper<expr::StepOp>(std::forward<LHSType>(lhs));
}



#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator>(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::GTOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator<(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::LTOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}




#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator>=(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::GTEOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator<=(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::LTEOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}




#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto operator==(LHSType&& lhs, RHSType&& rhs)
{
    return details::operator_helper<expr::EQOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



#if __cplusplus <= 201703L
template <class LHSType, class RHSType
        , class = std::enable_if_t<
            details::is_not_both_arithmetic_v<LHSType, RHSType> &&
            details::is_valid_op_param_v<LHSType> &&
            details::is_valid_op_param_v<RHSType>
        > >
#else
template <details::valid_op_param LHSType
        , details::valid_op_param RHSType>
requires details::not_both_arithmetic<LHSType, RHSType>
#endif
inline constexpr auto Sqrt(LHSType&& lhs,
	                            RHSType&& rhs)
{ 
    return details::operator_helper<expr::SqrtOp>(
            std::forward<LHSType>(lhs),
            std::forward<RHSType>(rhs));
}



} // namespace ppl
