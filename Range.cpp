#include "Range.hpp"
#include "Hand.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

vector<Hand>Range::range_to_list(vector<vector<int>> const matrix)
{
    // define card to match indexes of the matrix
    unordered_map<int, string> map_matrix_to_cards = {
        {0, "A"},
        {1, "K"},
        {2, "Q"},
        {3, "J"},
        {4, "T"},
        {5, "9"},
        {6, "8"},
        {7, "7"},
        {8, "6"},
        {9, "5"},
        {10, "4"},
        {11, "3"},
        {12, "2"}
    };

    vector<string> colors = {"h", "d", "c", "s"};
    vector<vector<string>> all_colors_combo = {
        {"h", "d"}, 
        {"h", "c"}, 
        {"h", "s"},
        {"d", "c"},
        {"d", "s"},
        {"c", "s"}
    };

    vector<Hand> vectorToReturn;

    for (int i=0; i<13; i++)
    {
        for (int j=0; j<13; j++)
        {
            // Case pair, i = j
            if (i==j)
            {
                if (matrix[i][j] > 0)
                {
                    for (auto color_combo : all_colors_combo)
                    {
                        string card1 = map_matrix_to_cards[i] + color_combo[0];
                        string card2 = map_matrix_to_cards[i] + color_combo[1];
                        array<string, 2> hand_to_add =  {card1, card2};
                        Hand h;
                        h.Cards = hand_to_add;
                        h.Frequency = matrix[i][j];

                        vectorToReturn.push_back(h);
                    }
                }
                
            }

            // Case suited, i < j
            if (i < j)
            {
                if (matrix[i][j] > 0)
                {
                    for (auto color : colors)
                    {
                        auto card1 = map_matrix_to_cards[i] + color;
                        auto card2 = map_matrix_to_cards[j] + color;
                        array<string, 2> hand_to_add =  {card1, card2};

                        Hand h;
                        h.Cards = hand_to_add;
                        h.Frequency = matrix[i][j];

                        vectorToReturn.push_back(h);
                    }

                    
                }
            }

            // Case offsuited, i > j
            if (i > j)
            {
                if (matrix[i][j] > 0)
                {
                    for (int k=0; k<(matrix[i][j] / 10); k++)
                    {
                        for (auto color_combo : all_colors_combo)
                        {
                            string card1 = map_matrix_to_cards[i] + color_combo[0];
                            string card2 = map_matrix_to_cards[j] + color_combo[1];
                            array<string, 2> hand_to_add1 =  {card1, card2};
                            Hand h1;
                            h1.Cards = hand_to_add1;
                            h1.Frequency = matrix[i][j];

                            string card3 = map_matrix_to_cards[j] + color_combo[0];
                            string card4 = map_matrix_to_cards[i] + color_combo[1];
                            array<string, 2> hand_to_add2 =  {card3, card4};
                            Hand h2;
                            h2.Cards = hand_to_add1;
                            h2.Frequency = matrix[i][j];

                            vectorToReturn.push_back(h1);
                            vectorToReturn.push_back(h2);
                        }

                    }
                }
            }
        }
    }

    // shuffle
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e(seed);
    shuffle(begin(vectorToReturn), end(vectorToReturn), e);

    return vectorToReturn; 
};