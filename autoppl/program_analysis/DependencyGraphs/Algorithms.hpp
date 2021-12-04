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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <vector>
#include <map>
#include <set>

#include <autoppl/program_analysis/DependencyGraphs/Graph.hpp>

using namespace std;

//graph assumed to be acyclic
vector<Node> GetAncestors(Graph g, Node n){

	vector<Node> ancestors = {};

	if (!g.HasParents(n)){
		ancestors.push_back(n);
		return ancestors;
	}
	else {
		vector<Node> parents = g.GetParents(n);
		for (auto parent : parents){
			parent_ancs = GetAncestors(g,parent);
		}
	}

}
