#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "Hand.hpp"
#include "Poker.hpp"

using namespace std;


class NodeStrategy
{
private:
    vector<double> RegretSum;
    vector<double> StrategySum;
    vector<double> Strategy;
    double ReachPr;
    double ReachPrSum;

    short int Visited;


public:
    NodeStrategy(int nbActions);
    ~NodeStrategy();
};

class StrategyMap
{
private:
    unordered_map<string, NodeStrategy> SM;
public:
    string GetNodeStrategyKey(string history, short int nbActions, Hand playerCard, short int player, PokerNode node);
    void UpdateStrategy();
    void UpdatePolicy(string key, short int player, double reachP0, double reachP1, vector<double> regrets);

    StrategyMap();
    ~StrategyMap();
};