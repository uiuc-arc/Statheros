#pragma once
#include <cmath>
#include <vector>
#include <autoppl/util/type_traits.hpp>
#include <fpm/fixed.hpp>
#include <fpm/math.hpp>

namespace ppl {
namespace math {

    // NATURAL LOG
    template <class T>								    
    T log(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::log(x); 
    }
   template <class T>								    
    T log(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::log(x); 
    }


    // EXP
    template <class T>								    
    T exp(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::exp(x); 
    }
    template <class T>								    
    T exp(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::exp(x); 
    }


    // SQRT
    template <class T>								    
    T sqrt(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::sqrt(x); 
    }
    template <class T>								    
    T sqrt(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::sqrt(x); 
    }


    // COS
    template <class T>								    
    T cos(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::cos(x); 
    }
    template <class T>								    
    T cos(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::cos(x); 
    }


    // POW
    template <class T>								    
    T pow(T base, T exp, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::pow(base, exp);
    }
    template <class T>								    
    T pow(T base, T exp, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr)
    {
        return fpm::pow(base, exp);
    }


    // FLOOR
    template <class T>								    
    T floor(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::floor(x); 
    }
    template <class T>								    
    T floor(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::floor(x); 
    }


    // CEIL
    template <class T>								    
    T ceil(T x, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::ceil(x); 
    }
    template <class T>								    
    T ceil(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::ceil(x); 
    }


    // is_int
    template <class T>								    
    T is_int(T x, std::enable_if_t<std::is_integral_v<T>>* = nullptr)
    {
        return true;
    }
    template <class T>								    
    T is_int(T x, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr)
    {
        return std::abs(std::ceil(x) - x) < 0.0000001;
    }
    template <class T>								    
    T is_int(T x, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        return fpm::is_int(x); 
    }


    // MAX
    template <class T>								    
    T max(T x, T y, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::max(x, y); 
    }
    template <class T>								    
    T max(T x, T y, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        if (x >= y) {
            return x;
        }
        return y;
    }


    // MIN
    template <class T>								    
    T min(T x, T y, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        return std::min(x, y); 
    }
    template <class T>								    
    T min(T x, T y, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr) 
    {
        if (x <= y) {
            return x;
        }
        return y;
    }


    // Mean of a vector
    template <class T>								    
    double mean(const std::vector<T>& data, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr)
    {
        double sum = 0;
        for (const auto& n : data) {
            sum += n;
        }
        return sum / data.size();
    }
    template <class T>								    
    T mean(const std::vector<T>& data, std::enable_if_t<ppl::is_fixed_point_v<T>>* = nullptr)
    {
        T avg(0);
        int t = 1;
        for (const auto& n : data) {
            avg += (n - avg) / t;
            ++t;
        }
        return avg;
    }

}  // namespace math
}  // namespace ppl
