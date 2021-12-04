#pragma once
#include <type_traits>
#include <variant>
#include <fpm/fixed.hpp>

#define DEFINE_ASSERT_ONE_PARAM(name) \
    namespace details {                         \
        template <bool b>                       \
        struct assert_##name                    \
        {                                       \
            static_assert(b,                    \
                    "Assert "#name" failed"  \
                    );                          \
        };                                      \
                                                \
        template<>                              \
        struct assert_##name<true> : std::true_type \
        {};                                     \
    }                                           \
    template <class T>                 \
    inline constexpr bool assert_##name =       \
        details::assert_##name<name<T>>::value; \

#define DEFINE_ASSERT_TWO_PARAM(name) \
    namespace details {                         \
        template <bool b>                       \
        struct assert_##name                    \
        {                                       \
            static_assert(b,                    \
                    "Assert "#name" failed"  \
                    );                          \
        };                                      \
                                                \
        template<>                              \
        struct assert_##name<true> : std::true_type \
        {};                                     \
    }                                           \
    template <class T, class U>                 \
    inline constexpr bool assert_##name =       \
        details::assert_##name<name<T, U>>::value; \
    
namespace ppl {

/**
 * Checks if type From can be explicitly converted to type To.
 */
template <class From, class To>
inline constexpr bool is_explicitly_convertible_v =
    std::is_constructible_v<To, From> &&
    !std::is_convertible_v<From, To>
    ;
DEFINE_ASSERT_TWO_PARAM(is_explicitly_convertible_v);

/**
 * Define is_arithmetic_v as including both int/float etc. and fixed-points.
 */
template <typename _Tp>
inline constexpr bool is_fixed_point_v = std::is_base_of_v<fpm::fixedpoint_base, _Tp>;

//template <typename _Tp>
//inline constexpr bool is_fixed_32_v = (is_fixed_point_v<_Tp> && fpm::is_32_base_v<_Tp>); //booleans short circuit so this should be ok


template <typename _Tp>
inline constexpr bool is_fixed_32_v = (is_fixed_point_v<_Tp> && ((std::alignment_of_v<_Tp>*8)==32));
//(std::is_same_v<typename _Tp::base_type,std::int32_t> || std::is_same_v<typename _Tp::base_type,int> ||
//std::is_same_v<typename _Tp::base_type,std::uint32_t> || std::is_same_v<typename _Tp::base_type,unsigned int>)); //booleans short circuit so this should be ok


template <typename _Tp>
inline constexpr bool is_fixed_64_v = (is_fixed_point_v<_Tp> && fpm::is_64_base_v<_Tp>);


template <typename _Tp>
inline constexpr bool is_arithmetic_v = std::is_arithmetic_v<_Tp> || is_fixed_point_v<_Tp>;

/**
 * Define variant with only unique types.
 * e.g. unique_variant<int, string, int> --> unique_variant<int, string>
 * Credits to: https://stackoverflow.com/questions/62108100/variant-with-unique-types
 */
template <typename T, typename... Ts>
struct filter_duplicates { using type = T; };

template <template <typename...> class C, typename... Ts, typename U, typename... Us>
struct filter_duplicates<C<Ts...>, U, Us...>
    : std::conditional_t<(std::is_same_v<U, Ts> || ...)
                       , filter_duplicates<C<Ts...>, Us...>
                       , filter_duplicates<C<Ts..., U>, Us...>> {};

template <typename T>
struct unique_variant;

template <typename... Ts>
struct unique_variant<std::variant<Ts...>> : filter_duplicates<std::variant<>, Ts...> {};

template <typename T>
using unique_variant_t = typename unique_variant<T>::type;

/**
 * Used for CRTP to unify certain expression types under one name.
 * CRTP types should simply derive from this base class.
 */
template <class T>
struct BaseCRTP 
{
    T& self() { return static_cast<T&>(*this); }
    const T& self() const { return static_cast<const T&>(*this); }
};

} // namespace ppl
