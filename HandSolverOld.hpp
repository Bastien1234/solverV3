#pragma once

#include <array>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>

class HandSolverOld
{
private:
    const std::array<std::string, 13> cards {"2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"};
    const std::array<std::string, 13> cards_low {"A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K"};
    const std::array<std::string, 4> colors {"h", "d", "c", "s"};

    std::unordered_map<std::string, double> hash_values;
    std::unordered_map<std::string, double> hash_values_low;
    
    // Below are values to reset

    std::array<double, 7> values_array;
    std::array<double, 7> values_array_low;

    bool possible_color {false};

    bool possible_straight_high {false};
    bool possible_straight_low {false};

    double straight_value_high {-1.0};
    double straight_value_low {-1.0};
    
    int color_spades {0};
    std::vector<double> list_spades_high;
    std::vector<double> list_spades_low;

    int color_clubs {0};
    std::vector<double> list_clubs_high;
    std::vector<double> list_clubs_low;

    int color_hearts {0};
    std::vector<double> list_hearts_high;
    std::vector<double> list_hearts_low;

    int color_diamonds {0};
    std::vector<double> list_diamonds_high;
    std::vector<double> list_diamonds_low;
    
public:
    HandSolverOld();
    ~HandSolverOld();
    
    void resetParams();

    double handValue(std::vector<std::string> const &arr);

};
