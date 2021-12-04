#pragma once
#include <math.h>       /* log */
#include <boost/uuid/uuid.hpp>
#include "UncertainIntervals/IntervalAnalysis.hpp"
#include <autoppl/expression/model/glue_node.hpp>
#include <map>
#include <set>
#include <iostream>
#include <utility>


using boost::uuids::uuid;

namespace globals {

std::map<uuid, std::set<uuid>> dependencies;
std::map<uuid, DeterministicInterval> intervals;
std::map<uuid, DeterministicInterval> single_likelihood_intervals; //max value of a SINGLE likelihood
std::map<uuid, DeterministicInterval> likelihood_intervals;	//interval bounding the SUM of all log likelihoods over ALL data samples
std::map<uuid, DeterministicInterval> biggest_intervals;

/*
void printDependencies(){
	for(auto elem : dependencies)
	{
	   std::cout << elem.first << " "   << "\n"; // << elem.second
	}
}*/

void printIntervals(){
	for(auto elem : intervals)
	{
	   std::cout << elem.first << " ["  << elem.second.lower << "," << elem.second.upper << "]" << "\n";
	}
}

void printSingleLikelihoodIntervals(){
	for(auto elem : single_likelihood_intervals)
	{
	   std::cout << elem.first << " ["  << elem.second.lower << "," << elem.second.upper << "]" << "\n";
	}
}



void printLikelihoodIntervals(){
	for(auto elem : likelihood_intervals)
	{
	   std::cout << elem.first << " ["  << elem.second.lower << "," << elem.second.upper << "]" << "\n";
	}
}


void printBiggestIntervals(){
	for(auto elem : biggest_intervals)
	{
	   std::cout << elem.first << " ["  << elem.second.lower << "," << elem.second.upper << "]" << "\n";
	}
}



} //end of globals::




namespace analysis {


//for a given number returns the number of integer bits needed to represent it
int numIntBits(double x){
	return std::max(1.,std::ceil(std::log2(x)));
}

int getValueTSize(){
	DeterministicInterval max_size_interval(0.,0.);

	for(auto elem : globals::intervals)
	{
	   //max_size_interval.print();
	   max_size_interval = union_(max_size_interval,elem.second);
	}

	//max_size_interval.print();
	double biggest = std::max(std::abs(max_size_interval.lower),std::abs(max_size_interval.upper));
	return numIntBits(biggest);
}



int getDistValueTSize(){
	DeterministicInterval max_size_interval(0.,0.);

	for(auto elem : globals::likelihood_intervals)
	{
	   max_size_interval = add(max_size_interval,elem.second);	//since we have to sum all log-likelihoods together
	}


	for(auto elem2 : globals::biggest_intervals)
	{
	   max_size_interval = union_(max_size_interval,elem2.second);	//since we union with the biggest interval we could have seen during intermediate computation steps
	}


	double biggest = std::max(std::abs(max_size_interval.lower),std::abs(max_size_interval.upper));

	return numIntBits(biggest);
}


int getSingleLikelihoodDistValueTSize(){
	DeterministicInterval max_size_interval(0.,0.);


	for(auto elem2 : globals::single_likelihood_intervals)
	{	
	   max_size_interval = union_(max_size_interval,elem2.second);	//since we union with the biggest interval we could have seen during intermediate computation steps
	}
	max_size_interval.print();

	double biggest = std::max(std::abs(max_size_interval.lower),std::abs(max_size_interval.upper));

	return numIntBits(biggest);
}




template <class LHSNodeType, class RHSNodeType>
std::pair<int,int> getBothSizes(ppl::expr::GlueNode<LHSNodeType, RHSNodeType>& model){
	model.getInterval();
	model.getLikelihoodInterval();
	int value_t = getValueTSize();
	int dist_value_t = getDistValueTSize();
	int single_likelihood_dist_value_t = getSingleLikelihoodDistValueTSize();

	std::cout << "Value_t needed: " << value_t << std::endl;
	std::cout << "Dist_Value_t needed: " << dist_value_t << std::endl;
	std::cout << "Single Likelihood Dist_Value_t needed (for Rem's optimization): " << single_likelihood_dist_value_t << std::endl;
	return std::pair<int,int>({value_t,dist_value_t});	
}

}
