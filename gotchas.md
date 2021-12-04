# Common Gotchas

The following is a list of Gotchas/errors that we encountered while setting up this repository.The aim of this file is to document common bugs that can happen if one doesn't fully understand the code or misspecifies things.


1) **Fixed point bugs**:
* When using doubles, `Uniform(0,1)` will essentially never sample 0 thus such a model may be fine for something like Stan but when using Fixed point one *can* sample 0 with non-negligible probability.
* Thus a model like this: `sigma |= Uniform(0,5)` can cause an error (if sigma must always be positive). Instead, the model should be restructured as `sigma |= Uniform(0.001, 5)`.
2) **Definition of "value_t"**: 
* The `value_t` referred to in the mh code (`autoppl/mcmc/mh.hpp`) is the type of the value in a node, which may be the type of the data that the user provides; this is distinct from the usage of `value_t` inside the distribution classes, which refers to the type that is obtained by sampling the distribution. 
 * Example: for ppl::Data the user could specify Q18.14 (value_t in a node), where sampling from a normal distribution could yield a Q16.16 (value_t in the normal distribution class, which would be cont_param_t in this case). 
 * This also explains the static_cast<value_t> in mh_fixed.hpp, wherein the value_t of the distribution is casted to the value_t of the eq_node.
3) **Fixed point casting**:
* Operations on two fixed point numbers with different fractional bits (but same basetype) result in a fixed point number with lower fractional bits. The only exceptions are when these operators are used: `static_cast<>, +=, -=, *=, /=`, in which case, the precision of the LHS variable is used.
* Truncation: If output is less precise when casting, it is rounded to the nearest precision.
* For Example:
    ```cpp
    fxp_8_prec / fxp_16_prec    // output has 8 fractional bits
    fxp_16_prec / fxp_8_prec    // output has 8 fractional bits

    fxp_8_prec /= fxp_16_prec   // output has 8 fractional bits
    fxp_16_prec /= fxp_8_prec   // output has 16 fractional bits

    static_cast<fxp_8_prec>(fxp_16_prec)   // output has 8 fractional bits
    static_cast<fxp_16_prec>(fxp_8_prec)   // output has 16 fractional bits
    ```
4) **Experimentation**:
* The number of mcmc samples need to be known at compile time. Hence, the variable must be initialized as `const int` .