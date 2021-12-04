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
using namespace std;


class Node {
	public:
		boost::uuids::uuid id;

	Node(boost::uuids::uuid id_){
		id = id_;
	}

};


class Graph{
	public:
	vector<Node> nodes;
	map<int,vector<Node> > AdjMap;	

	map<int,vector<Node> > InverseAdjMap;

        map<int,Node> conversionMap;
	map<boost::uuids::uuid,int> inverseConversionMap;
	int count;

	bool contains(Node n,vector<Node> vec){
		for (auto n_ : vec){
			if (n_.id == n.id){
				return true;
			}
		}
		return false;
	}

	void AddNode(Node n){

		if (contains(n,nodes)){
			return;
		}

		int curr_size = nodes.size();
		int IntID = curr_size+1;
		conversionMap.insert({IntID,n});
		inverseConversionMap.insert({n.id,IntID});

		vector<Node> children = {};
		AdjMap.insert({IntID,children});
		vector<Node> parents = {};
		InverseAdjMap.insert({IntID,parents});
	}


	void AddEdge(Node n1, Node n2){
		AddNode(n1);
		AddNode(n2);

		int n1IntID = inverseConversionMap[n1.id];
		int n2IntID = inverseConversionMap[n2.id];

		vector<Node> children = AdjMap[n1IntID];

		//if its already there, ignore
		if (contains(n2,children)){
			return;
		}
		else {
			AdjMap[n1IntID].push_back(n2);
			InverseAdjMap[n2IntID].push_back(n1);
		}

	}

	vector<Node> GetChildren(Node n){
		int nIntID = inverseConversionMap[n.id];
		return AdjMap[nIntID];
	}

	vector<Node> GetParents(Node n){
		int nIntID = inverseConversionMap[n.id];
		return InverseAdjMap[nIntID];
	}

	bool HasParents(Node n){
		int nIntID = inverseConversionMap[n.id];
		return (InverseAdjMap[nIntID].size()==0);
	}

};
