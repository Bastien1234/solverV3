#pragma once

#include "Poker.hpp"

class CFR
{
public:
    CFR();
    ~CFR(); // Remove everything from the tree at some point maybe ?

    double run(PokerNode *node);
    double runHelper(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance);
    double handleChanceNode(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance);
    double handlePlayerNode(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance);

    void UpdateTree(PokerNode *root);
    void DeleteTree(PokerNode *root);
};