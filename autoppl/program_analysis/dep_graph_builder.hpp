#pragma once
#include <cmath>
#include <cassert>
#include <random>
#include <autoppl/util/var_expr_traits.hpp>
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <autoppl/expression/variable/variable_viewer.hpp>
#include <fastad>
#include <autoppl/math/math.hpp>
#include <iostream>
#include <type_traits>





#if __cplusplus <= 201703L
template <class LHSNodeType, class RHSNodeType>
#else
template <util::model_expr LHSNodeType, util::model_expr RHSNodeType>
#endif
void BuildDeps_aux(ppl::expr::EqNode<LHSNodeType,RHSNodeType> e){


	//checks if the uuid is already in the graph

	//if not then add

	//add all dependencies

}



#if __cplusplus <= 201703L
template <class LHSNodeType, class RHSNodeType>
#else
template <util::model_expr LHSNodeType, util::model_expr RHSNodeType>
#endif
void BuildDeps(ppl::expr::GlueNode<LHSNodeType,RHSNodeType> g){

	//if the left child is an EqNode we call aux (everywhere we check isinstanceof we need a dummy parent class)
        if (std::is_base_of<LHSNodeType,ppl::expr::EqNodeParent>::value){
		BuildDeps_aux(g.getLeftChild());
        }
	//else if the left child is still a GlueNode we recurse
        else if (std::is_base_of<LHSNodeType,ppl::expr::GlueNodeParent>::value){
		BuildDeps(g.getLeftChild());
        }
	else {
		throw;
	};


	//if the right child is an EqNode we call aux (everywhere we check isinstanceof we need a dummy parent class)
        if (std::is_base_of<RHSNodeType,ppl::expr::EqNodeParent>::value){
		BuildDeps_aux(g.getRightChild());
        }
	//else if the right child is still a GlueNode we recurse
        else if (std::is_base_of<RHSNodeType,ppl::expr::GlueNodeParent>::value){
		BuildDeps(g.getRightChild());
        }
	else {
		throw;
	};
}
