#pragma once

#include "Hand.hpp"

#include <string>
#include <array>
#include <vector>

/*
    This class takes a n*n matrix as input with weighted combos
    Returns a shuffled list of hands from that matrix
    The second argument is here to slice this list
*/

class Range
{
private:

public:
    vector<Hand>range_to_list(std::vector<std::vector<int>> const matrix);
};