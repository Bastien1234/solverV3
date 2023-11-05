#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "Hand.hpp"

using namespace std;


// Constants
// -----------------
const int MaxRaises = 2;
const int Iterations1 = 100000;
const int HandsToKeepFromRange = 30;
static const int Pot = 75;
static const int EffectiveStack = 450;
const double Threashold = 0.6;
const int AllInSamplesize = 50;
const int MaxChanceNodes = 1500;

static const vector<string> board = {"Ah", "7d", "5h"};

// Strategies
const vector<double> OOPFlopBets = {0.4};
const vector<double> IPFlopBets = {0.25, 0.75, 1.2};
const vector<double> OOPFlopRaises = {3};
const vector<double> IPFlopRaises = {3};

const vector<double> OOPTurnBets = {0.8};
const vector<double> IPTurnBets = {0.75, 1.2};
const vector<double> OOPTurnRaises = {3};
const vector<double> IPTurnRaises = {3};

const vector<double> OOPRiverBets = {0.75};
const vector<double> IPRiverBets = {0.75};
const vector<double> OOPRiverRaises = {3};
const vector<double> IPRiverRaises = {3};

static const vector<vector<int>> MatrixOOP = {
	{0, 0, 0, 0, 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{0, 0, 50, 100, 100, 100, 100, 100, 0, 0, 0, 0, 0},
	{0, 50, 0, 10, 100, 100, 100, 100, 0, 0, 0, 0, 0},
	{50, 50, 100, 0, 100, 100, 100, 100, 0, 0, 0, 0, 0},
	{50, 100, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static const vector<vector<int>> MatrixIp = {
	{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
	{100, 100, 100, 100, 100, 100, 100, 0, 0, 0, 0, 0, 0},
	{100, 100, 100, 100, 100, 100, 0, 0, 0, 0, 0, 0, 0},
	{100, 100, 100, 100, 100, 100, 0, 0, 0, 0, 0, 0, 0},
	{100, 100, 100, 100, 100, 0, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0},
	{100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100},
};


// -----------------

const int chance = -1;
const int player0 = 0;
const int player1 = 1;

const string h_RootNode  = "R";
const string h_P0Deal    = "0";
const string h_P1Deal    = "1";
const string h_Chance    = "C";
const string h_Check     = "x";
const string h_CheckBack = "c";
const string h_Bet1      = "q";
const string h_Bet2      = "s";
const string h_Bet3      = "d";
const string h_Raise1    = "w";
const string h_Raise2    = "v";
const string h_AllIn     = "a";
const string h_Fold      = "f";
const string h_Call      = "k";

long getMemoValue(vector<string> array);
vector<vector<string>> getLimitedRunouts(int nbRunouts);



class PokerNode
{
private:
public:
    PokerNode *parent;
    int player;
    vector<double> probabilities;
    vector<vector<string>> limitedRunouts; // FIX ME: Kick that shit out to constants variables plz
    int turnIndex;
    short int Visited;
    int potSize;
    int effectiveSize;
    int currentFacingBet;
    int raiseLevel;
    int stage;
    vector<string> board;
    Hand p0Card;
    Hand p1Card;
    vector<double> RegretSum;
    vector<double> StrategySum;
    vector<double> Strategy;
    double ReachPr;
    double ReachPrSum;
    string history;
    vector<PokerNode*> children;
    vector<string> childrenHistory;
    PokerNode(
        int _player,
        vector<vector<string>> _limitedRunouts,
        int _potSize,
        int _effectiveSize,
        int _currentFacingBet,
        int _raiseLevel,
        int _stage,
        vector<string> _board,
        Hand _p0Card,
        Hand _p1Card,
        string _history
    );
    PokerNode(
        int _player,
        vector<vector<string>> _limitedRunouts,
        int _potSize,
        int _effectiveSize,
        int _currentFacingBet,
        int _raiseLevel,
        int _stage,
        vector<string> _board,
        Hand _p0Card,
        Hand _p1Card,
        string _history,
        int _turnIndex
    );
    ~PokerNode();
    int getPlayer();
    
    PokerNode getParent();
    char type();
    bool isTerminal();
    double utility(int player);
    Hand playerCard(int player);
    void instanciate();

    std::string computeCardHistory(int currentPlayer, std::string history);
};

class MasterMap
{
public:
    MasterMap();
    ~MasterMap();

    std::unordered_map<string, PokerNode*> map;
    void add(std::vector<PokerNode> children);
    PokerNode getNode(std::string);
    void Update();
};

// Let's do it C style

double getChildProbability(PokerNode *pokerNode, int i, MasterMap *masterMap);
int numChildren(PokerNode* pokerNode, MasterMap* masterMap);
PokerNode getChild(PokerNode* pokerNode, int i, MasterMap* masterMap);
std::vector<PokerNode> buildChildren(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildRootDeals(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildP0Deal(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildP1Deal(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildOpenAction(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildCBAction(PokerNode* pokerNode, MasterMap* masterMap);
std::vector<PokerNode> buildCFRAction(PokerNode* pokerNode, MasterMap* masterMap, bool isRaise);
std::vector<PokerNode> buildChanceNode(PokerNode* pokerNode, MasterMap* masterMap);

// Utils functions
vector<string> getFullBoard(vector<string> currentBoard, vector<string> player, vector<string>opponent);
vector<double> uniformDist(int n);
bool isOverThreasholdBet(const PokerNode *parent, double choice);
bool isOverThreasholdRaise(const PokerNode *parent, double choice);