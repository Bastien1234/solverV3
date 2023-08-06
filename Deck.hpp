#pragma once

#include <string>
#include <array>
#include <vector>


class Deck
{
private:
    std::vector<std::string> deck;
    std::vector<std::string> colors {"h", "d", "c", "s"};
    std::vector<std::string> cards {"A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K"};
public:
    Deck();
    ~Deck();
    std::vector<std::string> make_deck();
};