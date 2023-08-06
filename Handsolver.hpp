#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

using namespace std;


class Handsolver
{
private:
    vector<string> colors {"h", "d", "c", "s"};
    vector<string> cards {"A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K"};
    unordered_map<string, int> cardsValueMap = {
        { "A", 14 }, // Value 1 as well !
		{ "K", 13 },
		{ "Q", 12 },
		{ "J", 11 },
		{ "T", 10 },
		{ "9", 9 },
		{ "8", 8 },
		{ "7", 7 },
		{ "6", 6 },
		{ "5", 5 },
		{ "4", 4 },
		{ "3", 3 },
		{ "2", 2 },
    };

    unordered_map<int, unsigned short int> bitMaskStraightValues = {
        { 14, 31744 },
		{ 13, 15872 },
		{ 12, 7936 },
		{ 11, 3968 },
		{ 10, 1984 },
		{ 9,  992 },
		{ 8,  496 },
		{ 7,  248 },
		{ 6,  124 },
		{ 5,  62 },
    };

public:
    Handsolver();
    ~Handsolver();
    long solve(std::vector<std::string> arr);
};