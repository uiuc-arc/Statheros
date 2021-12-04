#pragma once
#ifdef DEBUG
#include <string>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <autoppl/settings.hpp>
#include <autoppl/config.hpp>
#include <array>

// this file is for storing global variables that can be tracked and updated while running experiments

namespace globals {

ppl::util::dist_value_t_default MAX_DIST_VALUE_T = std::numeric_limits<ppl::util::dist_value_t_default>::max();

int num_accepts = 0;
int num_samples = 0;
int early_exits = 0;

int err_before_warmup = 0;
int err_after_warmup = 0;

#ifdef DEBUG

#ifndef N_STACK_LEVELS
#define N_STACK_LEVELS 10
#endif

// These guys only look for integer overflows
int num_overflow_plus_equals = 0;
int num_overflow_minus_equals = 0;
int num_overflow_mult_equals = 0;


// to be opened by user.
FILE * plus_overflow_fp = NULL; // fopen("plus_overflow_" + std::to_string(PREC)    + ".trace","w");
FILE * minus_overflow_fp = NULL; // fopen("minus_overflow_" + std::to_string(PREC)    + ".trace","w");
FILE * mult_overflow_fp = NULL; // fopen("mult_overflow_" + std::to_string(PREC)    + ".trace","w");

/*
Utility function to free file pointers.

If the overflow_fp FILEs are allocated, make sure that this function is called before program is exited. 
This can be done like this:
```
int main() {
    atexit(globals::free_all);
    ...
}
```
*/
static void free_all(void)
{
    std::cout << "\nresults written to file..." << std::endl;
    fclose(plus_overflow_fp);
    fclose(minus_overflow_fp);
    fclose(mult_overflow_fp);
}
#endif // DEBUG





}
