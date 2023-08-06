#include "HandSolverOld.hpp"

#include <array>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>

#include <stdexcept>

HandSolverOld::HandSolverOld()
{
    
    
    // Populating hashmaps

    for (int i=0; i<13; i++)
    {
        double card_value = (double)i;
        hash_values[cards.at(i)] = card_value + 2.0;
    }

    for (int i=0; i<13; i++)
    {
        double card_value = (double)i;
        hash_values_low[cards_low.at(i)] = card_value + 1.0;
    }
    
    
}

HandSolverOld::~HandSolverOld()
{
}

void HandSolverOld::resetParams()
{
    possible_color = false;

    possible_straight_high = false;
    possible_straight_low = false;

    straight_value_high = -1.0;
    straight_value_low = -1.0;
    
    color_spades = 0;
    list_spades_high.clear();
    list_spades_low.clear();

    color_clubs = 0;
    list_clubs_high.clear();
    list_clubs_low.clear();

    color_hearts = 0;
    list_hearts_high.clear();
    list_hearts_low.clear();

    color_diamonds = 0;
    list_diamonds_high.clear();
    list_diamonds_low.clear();
}

double HandSolverOld::handValue(std::vector<std::string> const &arr)
{
    // Check if the input is correct
    
    /*
    std::set<std::string> set_of_array(std::begin(arr), std::end(arr));
    if (arr.size() != 7 || set_of_array.size() != 7)
    {
        std::cout << "wrong size of input" << std::endl;
        // throw new std::runtime_error(std::string{"Wrong input"});
        return -1.0;
    }

    for (auto i=0; i<arr.size(); i++)
    {
        auto card = arr.at(i).substr(0, 1);
        auto color = arr.at(i).substr(1, 2);

        auto ptr_arr_card = std::find(cards.begin(), cards.end(), card);
        auto ptr_arr_color = std::find(colors.begin(), colors.end(), color);

        if (ptr_arr_card == arr.end())
        {
            std::cout << "wrong cards input" << std::endl;
            // throw new std::runtime_error(std::string{"Wrong card input"});
            return -1.0;

        }

        if (ptr_arr_color == arr.end())
        {
            std::cout << "wrong color input" << std::endl;
            // throw new std::runtime_error(std::string{"Wrong color input"});
            return -1.0;

        }
    }
    */


    // Make the value array and sort the array
    for (int i=0; i<7; i++)
    {
        auto value_of_card = arr[i].substr(0, 1);
        values_array[i] = hash_values[value_of_card];
        values_array_low[i] = hash_values_low[value_of_card];
    }

    std::sort(values_array.begin(), values_array.end());
    std::sort(values_array_low.begin(), values_array_low.end());
    
    
    for (auto el : arr)
    {
        auto color = el.substr(1, 2);
        if (color == "h")
        {
            color_hearts++;
            double val_high_to_push = hash_values[el.substr(0, 1)];
            list_hearts_high.push_back(val_high_to_push);
            std::sort(list_hearts_high.begin(), list_hearts_high.end());
            double val_low_to_push = hash_values_low[el.substr(0, 1)];
            list_hearts_low.push_back(val_low_to_push);
            std::sort(list_hearts_low.begin(), list_hearts_low.end());
        }

        else if (color == "d")
        {
            color_diamonds++;
            double val_high_to_push = hash_values[el.substr(0, 1)];
            list_diamonds_high.push_back(val_high_to_push);
            std::sort(list_diamonds_high.begin(), list_diamonds_high.end());
            double val_low_to_push = hash_values_low[el.substr(0, 1)];
            list_diamonds_low.push_back(val_low_to_push);
            std::sort(list_diamonds_low.begin(), list_diamonds_low.end());
        }

        else if (color == "c")
        {
            color_clubs++;
            double val_high_to_push = hash_values[el.substr(0, 1)];
            list_clubs_high.push_back(val_high_to_push);
            std::sort(list_clubs_high.begin(), list_clubs_high.end());
            double val_low_to_push = hash_values_low[el.substr(0, 1)];
            list_clubs_low.push_back(val_low_to_push);
            std::sort(list_clubs_low.begin(), list_clubs_low.end());
        }

        else if (color == "s")
        {
            color_spades++;
            double val_high_to_push = hash_values[el.substr(0, 1)];
            list_spades_high.push_back(val_high_to_push);
            std::sort(list_spades_high.begin(), list_spades_high.end());
            double val_low_to_push = hash_values_low[el.substr(0, 1)];
            list_spades_low.push_back(val_low_to_push);
            std::sort(list_spades_low.begin(), list_spades_low.end());
        }
    }

    if (color_hearts >= 5
    || color_diamonds >= 5
    || color_clubs >= 5
    || color_spades >= 5)
    {
        possible_color = true;
    }
    
    // Check if possible straight

    /*
        The set_of_array variable has different size options :
        If it's under 5, there can't be any straight
        If it's 5 or over, we can check if now the straight is possible
    */

    std::set<double> set_set_of_values(std::begin(values_array), std::end(values_array));
    std::set<double> set_set_of_values_low(std::begin(values_array_low), std::end(values_array_low));
    std::vector<double> set_of_values {};
    std::vector<double> set_of_values_low {};

    for (auto it=set_set_of_values.begin(); it !=set_set_of_values.end(); it++)
    {
        set_of_values.push_back(*it);
    }

    for (auto it=set_set_of_values_low.begin(); it !=set_set_of_values_low.end(); it++)
    {
        set_of_values_low.push_back(*it);
    }

    
    if (set_of_values.size() >= 5)
    {
        auto iterations {set_of_values.size() - 4};
        for (int i=0; i<iterations; i++)
        {
            if (set_of_values[i]+1 == set_of_values[i+1]
            && set_of_values[i+1] +1 == set_of_values[i+2]
            && set_of_values[i+2] +1 == set_of_values[i+3]
            && set_of_values[i+3] +1 == set_of_values[i+4])
            {
                possible_straight_high = true;
                straight_value_high = set_of_values[i+4];
            }
        }

        if (possible_straight_high == false)
        {
            for (int i=0; i<iterations; i++)
            {
                if (set_of_values_low[i]+1 == set_of_values_low[i+1]
                && set_of_values_low[i+1] +1 == set_of_values_low[i+2]
                && set_of_values_low[i+2] +1 == set_of_values_low[i+3]
                && set_of_values_low[i+3] +1 == set_of_values_low[i+4])
                {
                    possible_straight_low = true;
                    straight_value_low = set_of_values_low[i+4];
                }
            }
        }
    }

    /*
        Check for straight flush
        returns 9 and the decimal is the value of the hand
    */
    
    if (possible_color == true && straight_value_high > 0)
    {
        std::vector<double> list_of_suits;

        if (color_hearts >= 5)
        {
            
            for (auto it = list_hearts_high.rbegin(); it != list_hearts_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_diamonds >= 5)
        {
            
            for (auto it = list_diamonds_high.rbegin(); it != list_diamonds_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_clubs >= 5)
        {
            
            for (auto it = list_clubs_high.rbegin(); it != list_clubs_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_spades >= 5)
        {
            
            for (auto it = list_spades_high.rbegin(); it != list_spades_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        double value_to_return{9.0};
        for (int i=list_of_suits.size()-5 ; i>=0; i--)
        {
            if (list_of_suits[i] == list_of_suits[i+4] + 4)
            {
                std::cout.precision(15);
                return value_to_return + (list_of_suits[i] * 0.01);
            } 
        }
    }

    if (possible_color == true && straight_value_low > 0)
    {
        std::vector<double> list_of_suits;

        if (color_hearts >= 5)
        {
            
            for (auto it = list_hearts_low.rbegin(); it != list_hearts_low.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_diamonds >= 5)
        {
            
            for (auto it = list_diamonds_low.rbegin(); it != list_diamonds_low.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_clubs >= 5)
        {
            
            for (auto it = list_clubs_low.rbegin(); it != list_clubs_low.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_spades >= 5)
        {
            
            for (auto it = list_spades_low.rbegin(); it != list_spades_low.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        double value_to_return{9.0};
        for (int i=list_of_suits.size()-5 ; i>=0; i--)
        {
            if (list_of_suits[i] == list_of_suits[i+4] + 4)
            {
                std::cout.precision(15);
                resetParams();
                return value_to_return + (list_of_suits[i] * 0.01);
            } 
        }
    }



   /*
    Check for four of a kind
    returns 8 then the value of the hand
   */

    if (set_of_values.size() < 5)
    {
        double quad_value;
        double best_kicker {-1.0};

        std::unordered_map<double, int> counter;
        for (auto el : values_array)
        {
            if (counter[el])
            {
                counter[el] += 1;
                if (counter[el] == 4) {
                    quad_value = el;
                    for (int i=0; i<set_of_values.size(); i++)
                    {
                        if (set_of_values[i] != el)
                        {
                            if (set_of_values[i] > best_kicker)
                            {
                                best_kicker = set_of_values[i];
                            }
                        }
                    }

                    std::cout.precision(15);
                    resetParams();
                    return 8.0 + (quad_value * 0.01) + (best_kicker * 0.0001);
                }
            }
            else
            {
                counter[el] = 1;
            }
        }
    }

    /*
        Check for full house
        returns 7 then the value of the hand
    */

   if (set_of_values.size() <= 4)
    {
       std::unordered_map<double, int> counter;
       double best_set {-1.0};
       double best_pair {-1.0};

       for (auto el : values_array)
        {
            // Make sure counter has a set and a pair at least to validate the full house combinaison
            if (counter[el])
            {
                counter[el] += 1;

                if (counter[el] == 3)
                {
                    if (el > best_set)
                    {
                        best_set = el;
                    }
                }
            }
            else
            {
                counter[el] = 1;
            }
        }

        for (auto el : values_array)
        {
            // To avoid overwriting of best pair in previous loop, we run another loop in the counter after the previous one
            if (counter[el] >= 2 && best_set != el)
            {
                if (el > best_pair)
                {
                    best_pair = el;
                }
            }
        }

        if (best_set > 0 && best_pair > 0)
        {
            std::cout.precision(15);
        double valueToReturn {7.0};
        valueToReturn += (best_set * 0.01);
        valueToReturn += (best_pair * 0.0001);

        resetParams();
        return valueToReturn;
        }
    }
    
    /*
        Flush returns 6
        Then the decimal is the value of the flush
    */

   if (possible_color)
    {
       std::vector<double> list_of_suits;

        if (color_hearts >= 5)
        {
            
            for (auto it = list_hearts_high.rbegin(); it != list_hearts_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_diamonds >= 5)
        {
            
            for (auto it = list_diamonds_high.rbegin(); it != list_diamonds_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_clubs >= 5)
        {
            
            for (auto it = list_clubs_high.rbegin(); it != list_clubs_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

        else if (color_spades >= 5)
        {
            
            for (auto it = list_spades_high.rbegin(); it != list_spades_high.rbegin() +5; it++)
            {
                list_of_suits.push_back(*it);
            }
        }

       double valueToReturn {6.0};
       double multiplier {1.0};

        for (auto it = list_of_suits.begin(); it!=list_of_suits.begin() +5; it++)
        {
            multiplier *= 0.01;
            valueToReturn += ((*it) * multiplier);
        }

        std::cout.precision(15);
        resetParams();
        return valueToReturn;
    }

    /*
        Check for straight
        returns 5 plus the straight value
    */

    if (possible_straight_high)
    {
        double valueToReturn {5.0};
        for (int i=set_of_values.size()-5 ; i>=0; i--)
        {
            if (set_of_values[i] +4 == set_of_values[i+4])
            return valueToReturn + (set_of_values[i+4] * 0.01);
        }
    }

    if (possible_straight_low)
    {
        double valueToReturn {6.0};
        for (int i=set_of_values_low.size()-5 ; i>=0; i--)
        {
            if (set_of_values_low[i] +4 == set_of_values_low[i+4])
            {
                resetParams();
                return valueToReturn + (set_of_values_low[i+4] * 0.01);
            }
                
        }
    }

    /*
        Three of a kind
        returns 4.0 then the decimals are the two best kickers
    */

   if (set_of_values.size() >=5)
    {
        std::unordered_map<double, int> counter;
        double valueToReturn {4.0};
        double best_set {-1.0};
        double best_kicker {-1.0};
        double best_second_kicker {-1.0};

        for (auto el : values_array)
        {
            if (counter[el])
            {
                counter[el] += 1;

                if (counter[el] == 3)
                    best_set = el;
            }
            else
            {
                counter[el] = 1;
            }
        }

        for (auto el : values_array)
        {
            if (el != best_set)
            {
                if (el > best_kicker)
                {
                    best_second_kicker = best_kicker;
                    best_kicker = el;
                }
                else if (el > best_second_kicker)
                    best_second_kicker = el;
            }
        }

        if (best_set > 0 && best_kicker > 0)
        {
            std::cout.precision(15);
            resetParams();
            return valueToReturn + (best_set * 0.01) + (best_kicker * 0.0001) + (best_second_kicker * 0.000001);
        } 
    }

    /*
        Two pairs
        returns 3.0 then the decimals are the values
    */

   if (set_of_values.size() <= 5)
    {
        std::unordered_map<double, int> counter;
        double valueToReturn {3.0};
        double best_pair {-1.0};
        double best_second_pair {-1.0};
        double best_kicker {-1.0};

        for (auto el : values_array)
        {
            if (counter[el])
            {
                counter[el] += 1;

                if (counter[el] == 2)
                {
                    if (el > best_pair)  
                    {
                        best_second_pair = best_pair;
                        best_pair = el;
                    }
                    else if (el > best_second_pair)
                        best_second_pair = el;   
                }
            }
            else
            {
                counter[el] = 1;
            }
        }
        for (auto el : values_array)
        {
            if (el != best_pair && el != best_second_pair)
            {
                if (el > best_kicker)
                    best_kicker = el;
            }
        }

        std::cout.precision(15);
        resetParams();
        return valueToReturn + (best_pair * 0.01) + (best_second_pair * 0.0001) + (best_kicker * 0.000001);
    }
    
    /*
        One pair
        Returns 2.0 then the values...you know the story, right ?
    */

    if (set_of_values.size() == 6)
    {   
        std::unordered_map<double, int> counter;
        double value_to_return {2.0};
        double pair {-1.0};

        for (auto el : values_array)
        {
            if (counter[el])
            {
                counter[el] += 1;

                if (counter[el] == 2)
                    pair = el;
            }
            else
            {
                counter[el] = 1;
            }
        }

        std::vector<double> non_pair_values;
        for (auto el : values_array)
        {
            if (el != pair)
            {
                non_pair_values.push_back(el);
            }
        }

        double multiplier {0.01};
        value_to_return += (pair * multiplier);

        for (auto it=non_pair_values.rbegin(); it !=non_pair_values.rbegin()+3; it++)
        {   
            multiplier *= 0.01;
            value_to_return += (multiplier * *it);
        }

        if (pair > 0)
        {
            std::cout.precision(15);
            resetParams();
            return value_to_return;
        } 
    }

    /*
        High card
        returns 1.0 then your terrible hand value afterwards
    */

   double high_card_value {1.0};
   double multiplier {1.0};
   for (auto it = values_array.rbegin(); it != values_array.rbegin() + 5; it ++)
    {   
        multiplier *= 0.01;
        high_card_value += (multiplier * *it);
    }

    std::cout.precision(15);
    resetParams();
    return high_card_value;
    
    
}