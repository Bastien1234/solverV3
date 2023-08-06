#pragma once

#include <string>
#include <array>
#include <vector>

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
const vector<double> OOPFlopBets = {0.25, 0.75};
const vector<double> IPFlopBets = {0.25, 0.75, 1.2};
const vector<double> OOPFlopRaises = {3};
const vector<double> IPFlopRaises = {3};

const vector<double> OOPTurnBets = {0.75, 1.2};
const vector<double> IPTurnBets = {0.75, 1.2};
const vector<double> OOPTurnRaises = {3};
const vector<double> IPTurnRaises = {3};

const vector<double> OOPRiverBets = {0.75, 1.2};
const vector<double> IPRiverBets = {0.75, 1.2};
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
    PokerNode *parent;
    int player;
    vector<PokerNode*> children;
    vector<double> probabilities;


    int raiseLevel;
    vector<string> board;

    int stage;

    vector<vector<string>> limitedRunouts; // FIX ME: Kick that shit out to constants variables plz
    int turnIndex;


    short int Visited;
public:
    int potSize;
    int effectiveSize;
    int currentFacingBet;
    Hand p0Card;
    Hand p1Card;
    vector<double> RegretSum;
    vector<double> StrategySum;
    vector<double> Strategy;
    double ReachPr;
    double ReachPrSum;
    string history;
    PokerNode(
        int _player,
        int _currentFacingBet,
        int _potSize,
        vector<string> _board,
        int _raiseLevel,
        int _stage,
        string _history,
        Hand _p0Card,
        Hand _p1Card,
        vector<vector<string>> _limitedRunouts
    );
    ~PokerNode();
    int getPlayer();
    int numChildren();
    PokerNode *getChild(int i);
    PokerNode *getParent();
    double getChildProbability(int i);
    char type();
    bool isTerminal();
    double utility(int player);
    Hand playerCard(int player);
    void instanciate();

    void buildChildren();
    void buildRootDeals();
    void buildP0Deal();
    void buildP1Deal();
    void buildOpenAction();
    void buildCBAction();
    void buildCFRAction(bool isRaise);
    void buildChanceNode();
};

// Utils functions
vector<string> getFullBoard(vector<string> currentBoard, vector<string> player, vector<string>opponent);
vector<double> uniformDist(int n);
bool isOverThreasholdBet(const PokerNode *parent, double choice);
bool isOverThreasholdRaise(const PokerNode *parent, double choice);