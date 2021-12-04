#pragma once
#include <autoppl/expression/model/eq_node.hpp>
#include <autoppl/expression/model/glue_node.hpp>

namespace ppl {

namespace details {

template <class ModelType>
struct get_n_params {};

template <class VarType, class DistType>
struct get_n_params<expr::EqNode<VarType, DistType>>
{
    static constexpr size_t value = 
#if __cplusplus <= 201703L
        1 * util::is_param_v<VarType>;
#else
        1 * util::param<VarType>;
#endif
};

template <class LHSNodeType, class RHSNodeType>
struct get_n_params<expr::GlueNode<LHSNodeType, RHSNodeType>>
{
    static constexpr size_t value =
        get_n_params<LHSNodeType>::value +
        get_n_params<RHSNodeType>::value;
};

} // namespace details

template <class ModelType>
inline constexpr size_t get_n_params_v =
    details::get_n_params<ModelType>::value;

} // namespace ppl
