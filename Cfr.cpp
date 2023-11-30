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
#include "Logs.hpp"

#include "backward.hpp"

double getSign(int player0, int player1) {
    if (player0 == player1) { return 1.0; }
    return -1.0;
}

CFR::CFR() {};
CFR::~CFR() 
{};

double CFR::run(PokerNode node, MasterMap *masterMap)
{
    text_log(LOGS_ENABLED, "CFR::run");
    return this->runHelper(node, node.getPlayer(), 1.0, 1.0, 1.0, masterMap);
}

double CFR::runHelper(PokerNode node, int lastPlayer, double reachP0, double reachP1, double reachChance, MasterMap *masterMap)
{
    text_log(LOGS_ENABLED, "CFR::runHelper-in");
    double ev;
    switch (node.type()) {
        case 't':
            ev = node.utility(lastPlayer);
        break;

        case 'c':

            ev = this->handleChanceNode(node, lastPlayer, reachP0, reachP1, reachChance, masterMap);
        break;

        case 'p':

            auto sgn = getSign(lastPlayer, node.getPlayer());
            ev = sgn * this->handlePlayerNode(node, lastPlayer, reachP0, reachP1, reachChance, masterMap);
            break;
    }
    text_log(LOGS_ENABLED, "CFR::runHelper-out");
    return ev;
}

double CFR::handleChanceNode(PokerNode node, int lastPlayer, double reachP0, double reachP1, double reachChance, MasterMap *masterMap)
{
    int nbChildren = numChildren(&node, masterMap);
    srand((unsigned) time(NULL));
    int random = rand() % nbChildren;
    auto child = getChild(&node, random, masterMap);
    // auto p = (double)getChildProbability(&node, random, masterMap);
    // double ev = p * this->runHelper(child, lastPlayer, reachP0, reachP1, reachChance*p, masterMap);
    double ev = this->runHelper(child, lastPlayer, reachP0, reachP1, reachChance, masterMap);

    return ev;
}

double CFR::handlePlayerNode(PokerNode node, int lastPlayer, double reachP0, double reachP1, double reachChance, MasterMap *masterMap)
{
    auto player = node.getPlayer();
    int nbChildren = numChildren(&node, masterMap);

    if (nbChildren == 1) {
        auto child = getChild(&node, 0, masterMap);
        return this->runHelper(child, player, reachP0, reachP1, reachChance, masterMap);
    }

    Hand playerCard;

    if (player == 0) {
        playerCard = node.p0Card;
    } else if (player == 1) {
        playerCard = node.p1Card;
    }

    auto strategy = node.Strategy;
    auto actionUtils = FilledArrayDouble(nbChildren, 0.0);

    if (nbChildren == 0) {
        std::cout << "WOOOOOOOO" << std::endl;
        // FIX ME LOL
        
    }

    for (int i = 0; i<nbChildren; i++)
    {
        auto child = getChild(&node, i, masterMap);
        auto p = strategy[i];
        if (player == 0) {
            auto comboFrequency = node.p0Card.Frequency;
            actionUtils[i] = this->runHelper(child, player, comboFrequency * p * reachP0, reachP1, reachChance, masterMap);
        } else {
            auto comboFrequency = node.p1Card.Frequency;
            actionUtils[i] = this->runHelper(child, player, reachP0, comboFrequency * p * reachP1, reachChance, masterMap);
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
        node.ReachPr += reachP0;
        for (int i = 0; i<node.RegretSum.size(); i++)
        {
            node.RegretSum[i] += reachP1 * regrets[i];
        }
    }

    else {
        node.ReachPr += reachP1;
        for (int i = 0; i<node.RegretSum.size(); i++)
        {
            node.RegretSum[i] += reachP0 * regrets[i];
        }
    }

    return util;
}