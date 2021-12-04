#pragma once
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <autoppl/settings.hpp>

// this file is for storing global variables that can be tracked and updated while running experiments

namespace globals {

std::vector< ppl::util::cont_param_t > xs;
std::vector< ppl::util::cont_param_t > ys;

}
