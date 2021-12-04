#pragma once
#include <autoppl/util/var_traits.hpp>
#include <autoppl/util/dist_expr_traits.hpp>
#include <vector>
#include <initializer_list>
#include <cassert>
#include <autoppl/math/math.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <autoppl/program_analysis/dependencies_intervals.hpp>
#include <autoppl/program_analysis/UncertainIntervals/IntervalAnalysis.hpp>


namespace ppl {


template <class ValueType>
struct Param : util::ParamLike<Param<ValueType>> {
    using value_t = ValueType;
    using pointer_t = value_t*;
    using const_pointer_t = const value_t*;

    Param(value_t value, pointer_t storage_ptr) noexcept
        : value_{value}, storage_ptr_{storage_ptr},tag(boost::uuids::random_generator()()), infer_value_{storage_ptr != nullptr}
    {}

    Param(pointer_t storage_ptr) noexcept
        : Param(0., storage_ptr) {}

    Param(value_t value) noexcept
        : Param(value, nullptr) {}

    Param() noexcept
        : Param(0., nullptr) {}

    void set_value(value_t value) { value_ = value; }

    constexpr size_t size() const { return 1; }
    value_t get_value(size_t = 0) const {
        return value_;
    }

    void set_storage(pointer_t storage_ptr) { storage_ptr_ = storage_ptr; }
    pointer_t get_storage() { return storage_ptr_; }
    const_pointer_t get_storage() const { return storage_ptr_; }

    bool infer_value() { return infer_value_; }

    boost::uuids::uuid getUUID(){
	    return tag;
    }

    std::set<boost::uuids::uuid> getDeps(){
        std::set<boost::uuids::uuid> used_vars; 
        used_vars.insert(tag); //variable viewer is calling variable's function
        return used_vars;
    }

    bool isData(){ return false; }

private:
    value_t value_;  // store value associated with var
    pointer_t storage_ptr_;        // points to beginning of storage
                                   // storage is assumed to be contiguous
    boost::uuids::uuid tag;
    bool infer_value_;
};


template <class ValueType>
struct Data : util::DataLike<Data<ValueType>>
{
    using value_t = ValueType;
    using pointer_t = value_t*;
    using const_pointer_t = const value_t*;

    template <typename iterator>
    Data(iterator begin, iterator end) noexcept
        : values_{begin, end},tag(boost::uuids::random_generator()()) {}

    Data(std::initializer_list<value_t> values) noexcept
        : Data(values.begin(), values.end()) {}

    Data(value_t value) noexcept
        : values_{{value}},tag(boost::uuids::random_generator()()) {}

    Data() noexcept : values_{},tag(boost::uuids::random_generator()()) {}

    size_t size() const { return values_.size(); }

    value_t get_value(size_t i) const { 
        assert((i >= 0) && (i < size()));  
        return values_[i]; 
    }

    void observe(value_t value) { values_.push_back(value); }
    void clear() { values_.clear(); }

    auto begin() const { return values_.begin(); }
    auto end() const { return values_.end(); }

    value_t mean() { return math::mean(values_); }


    boost::uuids::uuid getUUID(){
	    return tag;
    }

    std::set<boost::uuids::uuid> getDeps(){
        std::set<boost::uuids::uuid> used_vars; 
        used_vars.insert(tag); //variable viewer is calling variable's function
        return used_vars;
    }

    bool isData(){ return true;}

    DeterministicInterval getDataInterval() {
        value_t min_ = value_t(0);
        value_t max_ = value_t(0);
        for (int i = 0; i < size(); i++) {
            value_t v = get_value(i);
            if (v < min_) {
                min_=v;
            } else if (v>max_) {
                max_=v;
            }
        }

        return DeterministicInterval(double(min_),double(max_));
    }

private:
    std::vector<value_t> values_;  // store value associated with var
    boost::uuids::uuid tag;
};

// Useful aliases
using cont_var = Data<util::cont_param_t>; // continuous RV var
using disc_var = Data<util::disc_param_t>; // discrete RV var

}  // namespace ppl
