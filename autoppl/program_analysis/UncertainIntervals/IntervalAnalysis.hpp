#pragma once
#include <algorithm>    // std::max
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <math.h>       /* log */
#include <assert.h> 
#include <limits>



class DeterministicInterval {

	public:
		double lower;
		double upper;

	DeterministicInterval(double lower_, double upper_){
		lower = lower_;
		upper = upper_;
	}

	bool containsZero(){
		return ((lower<=0) && (upper>=0));
	}

	void print(){
		std::cout << "my interval is: [" << lower << "," << upper << "]" << std::endl;
	}

};


DeterministicInterval add(DeterministicInterval a, DeterministicInterval b){
	DeterministicInterval res(a.lower+b.lower,a.upper+b.upper);
	return res;
}


DeterministicInterval sub(DeterministicInterval a, DeterministicInterval b){
	DeterministicInterval res(a.lower-b.lower,a.upper-b.upper);
	return res;
}


DeterministicInterval mult(DeterministicInterval a, DeterministicInterval b){

	double p1 = a.lower * b.lower;
	double p2 = a.lower * b.upper;
	double p3 = a.upper * b.lower;
	double p4 = a.upper * b.upper;

	double min_ = std::min(std::min(p1,p2),std::min(p3,p4));
	double max_ = std::max(std::max(p1,p2),std::max(p3,p4));

	DeterministicInterval res(min_,max_);
	return res;
}





DeterministicInterval max(DeterministicInterval a, DeterministicInterval b){

	double min_ = std::max(a.lower,b.lower);
	double max_ = std::max(a.upper,b.upper);

	DeterministicInterval res(min_,max_);
	return res;
}

DeterministicInterval div(DeterministicInterval a, DeterministicInterval b){

	if (!b.containsZero()){
		DeterministicInterval inverse = DeterministicInterval(1./b.upper,1./b.lower);
		return mult(a,inverse);		
	}
	else {
		DeterministicInterval res(std::numeric_limits<double>::min(),std::numeric_limits<double>::max());
	}
}




DeterministicInterval sqrt_prod(DeterministicInterval a, DeterministicInterval b){

	DeterministicInterval prod = mult(a,b);
	double min_ = std::max(prod.lower,0.);
	assert(min_>0.0);
	double max_ = std::max(prod.upper,0.);
	assert(min_<max_);
	DeterministicInterval res(std::sqrt(min_),std::sqrt(max_));
	return res;
}



DeterministicInterval log_(DeterministicInterval a){

	DeterministicInterval res(std::log(a.lower),std::log(a.upper));
	return res;
}


DeterministicInterval exp_(DeterministicInterval a){

	DeterministicInterval res(std::exp(a.lower),std::exp(a.upper));
	return res;
}


DeterministicInterval square(DeterministicInterval a){
	DeterministicInterval res = mult(a,a);
	res.lower = std::max(0.,res.lower);
	return res;
}


DeterministicInterval scale(double c, DeterministicInterval a){
	if (c>0.){
		DeterministicInterval res(a.lower*c,a.upper*c);
		return res;
	}
	else if (c<0.){
		DeterministicInterval res(a.upper*c,a.lower*c);
		return res;
	}
	else {
		DeterministicInterval res(0.,0.);
		return res;
	}
}


DeterministicInterval constant_(double c){

	DeterministicInterval res(c,c);
	return res;

}


DeterministicInterval union_(DeterministicInterval a, DeterministicInterval b){
	DeterministicInterval res(std::min(a.lower,b.lower),std::max(a.upper,b.upper));
	return res;
}


