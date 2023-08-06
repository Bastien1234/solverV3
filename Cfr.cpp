#include "Cfr.hpp"

#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <stdio.h>

#include "Poker.hpp"
#include "Hand.hpp"
#include "Utils.hpp"

double getSign(int player0, int player1) {
    if (player0 == player1) { return 1.0; }
    return -1.0;
}

CFR::CFR() {};
CFR::~CFR() {};

double CFR::run(PokerNode *node)
{
    return this->runHelper(node, node->getPlayer(), 1.0, 1.0, 1.0);
}

double CFR::runHelper(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance)
{
    printf("Into run helper\t node histo : %s\n", node->history.c_str());
    printf("Cards\n p0 : %s %s\np1 : %s %s\n", node->p0Card.Cards.at(0).c_str(), node->p0Card.Cards.at(1).c_str(), node->p1Card.Cards.at(0).c_str(), node->p1Card.Cards.at(0).c_str());

    double ev;
    switch (node->type()) {
        case 't':
        printf("cast t\n");
            ev = node->utility(lastPlayer);
        break;

        case 'c':
        printf("cast c\n");

            ev = this->handleChanceNode(node, lastPlayer, reachP0, reachP1, reachChance);
        break;

        case 'p':
        printf("cast p\n");

            auto sgn = getSign(lastPlayer, node->getPlayer());
            ev = sgn * this->handlePlayerNode(node, lastPlayer, reachP0, reachP1, reachChance);
            break;
    }

    return ev;
}

double CFR::handleChanceNode(PokerNode *node, int lastPlayer, double reachP0, double reachP1, double reachChance)
{
    printf("Into CHANCE\t node histo : %s\n", node->history.c_str());
        printf("1\n");

    int nbChildren = node->numChildren();
        printf("2\n");

    srand((unsigned) time(NULL));
        printf("3\n");

    int random = rand() % nbChildren;
        printf("4\n");

    auto child = node->getChild(random);
        printf("5\n");

    auto p = (double)node->getChildProbability(random);
        printf("6\n");
    double ev = p * this->runHelper(child, lastPlayer, reachP0, reachP1, reachChance*p);
        printf("7\n");

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
        printf("Problem m m m m m mmmmmmm \n");
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

void CFR::UpdateTree(PokerNode *root) 
{
    // ...
}
void CFR::DeleteTree(PokerNode *root)
{
    // ...
}

