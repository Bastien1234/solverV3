#include "Cfr.hpp"

#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <queue>

#include "Poker.hpp"
#include "Hand.hpp"
#include "Utils.hpp"

#include "backward.hpp"

double getSign(int player0, int player1) {
    if (player0 == player1) { return 1.0; }
    return -1.0;
}

CFR::CFR() {};
CFR::~CFR() 
{
    for (auto kv : this->cfr_map)
    {
        auto node = kv.second;
        delete(node);
    }
};

double CFR::run(PokerNode *node)
{
    return this->runHelper(node, node->getPlayer(), 1.0, 1.0, 1.0);
}

double CFR::runHelper(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance)
{
    double ev;
    switch (node->type()) {
        case 't':
            ev = node->utility(lastPlayer);
        break;

        case 'c':

            ev = this->handleChanceNode(node, lastPlayer, reachP0, reachP1, reachChance);
        break;

        case 'p':

            auto sgn = getSign(lastPlayer, node->getPlayer());
            ev = sgn * this->handlePlayerNode(node, lastPlayer, reachP0, reachP1, reachChance);
            break;
    }

    return ev;
}

double CFR::handleChanceNode(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance)
{
    int nbChildren = node->numChildren();
    srand((unsigned) time(NULL));
    int random = rand() % nbChildren;
    auto child = node->getChild(random);
    auto p = (double)node->getChildProbability(random);
    double ev = p * this->runHelper(child, lastPlayer, reachP0, reachP1, reachChance*p);

    return ev;
}

double CFR::handlePlayerNode(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance)
{
    auto player = node->getPlayer();
    int nbChildren = node->numChildren();

    if (nbChildren == 1) {
        auto child = node->getChild(0);
        return this->runHelper(child, player, reachP0, reachP1, reachChance);
    }

    Hand playerCard;

    if (player == 0) {
        playerCard = node->p0Card;
    } else if (player == 1) {
        playerCard = node->p1Card;
    }

    auto strategy = node->Strategy;
    auto actionUtils = FilledArrayDouble(nbChildren, 0.0);

    if (nbChildren == 0) {
        // FIX ME LOL
    }

    for (int i = 0; i<nbChildren; i++)
    {
        auto child = node->getChild(i);
        auto p = strategy[i];
        if (player == 0) {
            actionUtils[i] = this->runHelper(child, player, p*reachP0, reachP1, reachChance);
        } else {
            actionUtils[i] = this->runHelper(child, player, reachP0, p*reachP1, reachChance);
        }
    }

    double util = 0.0;
    for (int i = 0; i<actionUtils.size(); i++) {
        util += actionUtils[i] * strategy[i];
    }

    vector<double> regrets(nbChildren);
    fill(regrets.begin(), regrets.end(), 0.0);

    for (int i = 0; i<regrets.size(); i++) 
    {
        auto candidateRegret = actionUtils[i] - util;
        if (candidateRegret > 0) {
            regrets[i] = candidateRegret;
        }
        else { regrets[i] = 0.0; }
    }

    // Update policy
    if (player == 0)
    {
        node->ReachPr += reachP0;
        for (int i = 0; i<node->RegretSum.size(); i++)
        {
            node->RegretSum[i] += reachP1 * regrets[i];
        }
    }

    else {
        node->ReachPr += reachP1;
        for (int i = 0; i<node->RegretSum.size(); i++)
        {
            node->RegretSum[i] += reachP0 * regrets[i];
        }
    }

    return util;
}

void CFR::UpdateTree() 
{

    for (auto kv : this->cfr_map)
    {
        auto node = kv.second;

        // Update strategy
        for (int i=0; i<node->StrategySum.size(); i++)
        {
            node->StrategySum[i] = node->ReachPr * node->Strategy[i];
        }

        node->ReachPrSum += node->ReachPr;

        // Get Strategy
        double normalizingSum = 0.0;
        vector<double> regrets;

        for (int i=0; i<node->RegretSum.size(); i++)
        {
            regrets.push_back(node->RegretSum[i]);
            normalizingSum += node->RegretSum[i];
        }

        for (int index=0; index<regrets.size(); index++)
        {
            if (normalizingSum > 0) {
                regrets[index] = regrets[index] / normalizingSum;
            } else {
                regrets[index] = 1.0 / double(regrets.size());
            }
        }

        node->Strategy = regrets;

        node->ReachPr = 0.0;
    }
};
