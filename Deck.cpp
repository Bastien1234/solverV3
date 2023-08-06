#include "Deck.hpp"

#include <algorithm>
#include <random>
#include <chrono>
#include <vector>

Deck::Deck()
{
    for (auto card : cards)
    {
        for (auto color : colors)
        {
            deck.push_back(card+color);
        }
    }
}

Deck::~Deck()
{
}

std::vector<std::string> Deck::make_deck()
{
    // Suffling the deck
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(std::begin(deck), std::end(deck), e);
   
    return deck;
}