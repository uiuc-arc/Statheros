#pragma once
#include <algorithm>
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/var_expr_traits.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <autoppl/program_analysis/dependencies_intervals.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>


namespace ppl {
namespace expr {


struct VarViewerParent{}; 

#if __cplusplus <= 201703L
template <class VariableType>
#else
template <util::var VariableType>
#endif
struct VariableViewer : util::VarExpr<VariableViewer<VariableType>>, VarViewerParent
{
#if __cplusplus <= 201703L
    static_assert(util::assert_is_var_v<VariableType>);
#endif

    using var_t = VariableType;
    using value_t = typename util::var_traits<var_t>::value_t;

    VariableViewer(var_t& var)
        : var_ref_{var}
    {}

    void printName(){
	var_ref_.get().printName();	//have to use get because of std::reference_wrapper
    }

    void printAddress(){
	std::cout << &var_ref_.get() << std::endl;  //have to use get because of std::reference_wrapper
    }


    bool isData(){
	return (var_ref_.get()).getData();
    }

    boost::uuids::uuid getUUID(){
	return (var_ref_.get()).getUUID();
    }

    std::set<boost::uuids::uuid> getDeps(){
	/*std::set<boost::uuids::uuid> used_vars; 
	used_vars.insert(var_ref_.get().getUUID()); //variable viewer is calling variable's function
	return used_vars;*/
	return var_ref_.get().getDeps();
    }

   DeterministicInterval getInterval(){
	boost::uuids::uuid my_id = getUUID();
	//return globals::intervals[my_id];
	//https://stackoverflow.com/questions/10699689/how-can-i-get-a-value-from-a-map
	return globals::intervals.find(my_id)->second; //can lead to undefined behavior, but only thing that works...
   }


    value_t get_value(size_t i = 0) const { return var_ref_.get().get_value(i); }
    size_t size() const { return var_ref_.get().size(); }


private:
    using var_ref_t = std::reference_wrapper<var_t>;
    var_ref_t var_ref_;
};

} // namespace expr
} // namespace ppl
