// Once node is created, push it onto the map directly to avoid recreation and only one map for everything 

#include "Poker.hpp"

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include <stdio.h> // DEBUG

#include "Hand.hpp"
#include "Handsolver.hpp"
#include "Deck.hpp"
#include "NodeStrategy.hpp"

#include "backward.hpp"

using namespace std;

unordered_map<string, long> memoMap;
Handsolver handsolver;
Deck deckMaker;

long getMemoValue(vector<string> array)
{
    auto sorted = array;
    sort(begin(sorted), end(sorted));
    string final = "";
    for (auto el : sorted)
    {
        final += el;
    }

    if (memoMap.find(final) != memoMap.end())
    {
        return memoMap[final];
    }
    else 
    {
        memoMap[final] = handsolver.solve(array);
        return memoMap[final];
    }
}

vector<vector<string>> getLimitedRunouts(int nbRunouts)
{
    vector<vector<string>> runouts;
    while (true)
    {
        vector<string> runout;
        auto deck = deckMaker.make_deck();

        for (int i=0; i<deck.size(); i++)
        {
            if (find(board.begin(), board.end(), deck[i]) == board.end())
            {
                runout.push_back(deck[i]);
                if (runout.size() == 2)
                {
                    runouts.push_back(runout);
                    continue;
                }
            }
        }

        if (runouts.size() == nbRunouts)
        {
            return runouts;
        }
    }
}

PokerNode::PokerNode(
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
        string _history)
        : 
        player(_player),
        limitedRunouts(_limitedRunouts),
        potSize(_potSize),
        effectiveSize(_effectiveSize),
        currentFacingBet(_currentFacingBet),
        raiseLevel(_raiseLevel),
        stage(_stage),
        board(_board),
        p0Card(_p0Card),
        p1Card(_p1Card),
        history(_history)
{
    this->turnIndex = -1;
};

PokerNode::~PokerNode(){};

int PokerNode::getPlayer()
{
    return this->player;
}

int PokerNode::numChildren()
{
    if (this->children.size() == 0)
    {
        this->buildChildren();
    }

    return this->children.size();
}

PokerNode* PokerNode::getChild(int i)
{
    if (this->children.size() == 0)
    {
        this->buildChildren();
    }

    return this->children.at(i);
}

PokerNode* PokerNode::getParent()
{
    return this->parent;
}

double PokerNode::getChildProbability(int i)
{
    return this->probabilities.at(i);
}

char PokerNode::type()
{
    if (this->isTerminal() == true)
    {
        return 't';
    }
    else if (this->player == -1)
    {
        return 'c';
    }

    return 'p';
}

bool PokerNode::isTerminal()
{

    if (this->history.size() == 0)
    {
        return false;
    }

    int lastElementIndex = this->history.size() -1;

    string lastAction = this->history.substr(lastElementIndex);
    
    if (lastAction == h_Fold)
    {
        return true;
    }

    if (this->history.size() >2) 
    {
        char areWeAllIn = this->history[this->history.size() - 2];
        if (areWeAllIn == 'a')
        {
            return true;
        }
    }

    if (this->stage == 2 && (lastAction == h_CheckBack || lastAction == h_Call))
    {
        return true;
    }
    
    return false;
}

Hand PokerNode::playerCard(int player)
{
    if (player == 0)
    {
        return this->p0Card;
    }

    return this->p1Card;
}

vector<string> getFullBoard(vector<string> currentBoard, array<string, 2> player, array<string, 2>opponent)
{
    auto deck = deckMaker.make_deck();
    vector<string> cardsOut;
    for (auto el : player) {
        cardsOut.push_back(el);
    }

    for (auto el : opponent) {
        cardsOut.push_back(el);
    }

    vector<string> availableCards;

    for (auto card : deck) {
        if (find(deck.begin(), deck.end(), card) == deck.end()) {
            availableCards.push_back(card);
            if (availableCards.size() > 1) {
                break;
            }
        }
    }

    vector<string> fullBoard;

    if (currentBoard.size() == 3) {
        fullBoard.push_back(availableCards.at(0));
        fullBoard.push_back(availableCards.at(1));
    } else {
        fullBoard.push_back(availableCards.at(0));
    }

    return fullBoard;
}

double PokerNode::utility(int player)
{
    Hand cardPlayer = this->playerCard(player);
    Hand cardOpponent = this->playerCard(1 - player);


    bool isShowdown;

    string lastAction = this->history.substr(this->history.size() -1);

    if (lastAction == h_Fold)
    {
        isShowdown = false;
    } else {
        isShowdown = true;
    }

    if (isShowdown == false) {

        // FIX ME
			// return -float64(n.PotSize) * opponent or hero card frequency
			// Also increase pot size on bets ?
        if (this->player == player) {
            return (double)this->potSize;
        } else {
            return -(double)this->potSize;
        }
    }

    // Case river showdown
    if (this->stage == 2) {

        vector<string> playerFinalHand = board;
        playerFinalHand.push_back(cardPlayer.Cards.at(0));
        playerFinalHand.push_back(cardPlayer.Cards.at(1));


        vector<string> opponentFinalhand = board;
        opponentFinalhand.push_back(cardOpponent.Cards.at(0));
        opponentFinalhand.push_back(cardOpponent.Cards.at(1));

        long playerHandValue = handsolver.solve(playerFinalHand);
        long opponentHandValue = handsolver.solve(opponentFinalhand);



        if (playerHandValue > opponentHandValue) {
            return (double)this->potSize;
        } 
        else if (opponentHandValue > playerHandValue) {
            return - (double)this->potSize;
        } else {
            return 0.0;
        }
    }

    // Case flop and turn showdown
    // We do a sampling of outcomes to avoid calculate every possible turn/river
    double cumulativePlayerWinnings = 0.0;
    double averagePlayerWinnings = 0.0;

    for (int i = 0; i<AllInSamplesize; i++) {
        auto fullBoard = getFullBoard(this->board, cardPlayer.Cards, cardOpponent.Cards);

        vector<string> playerFinalHand = fullBoard;
        playerFinalHand.push_back(cardPlayer.Cards.at(0));
        playerFinalHand.push_back(cardPlayer.Cards.at(1));

        vector<string> opponentFinalhand = fullBoard;
        opponentFinalhand.push_back(cardOpponent.Cards.at(0));
        opponentFinalhand.push_back(cardOpponent.Cards.at(1));

        auto playerHandValue = handsolver.solve(playerFinalHand);
        auto opponentHandValue = handsolver.solve(opponentFinalhand);

        if (playerHandValue > opponentHandValue) {
            cumulativePlayerWinnings += (double)this->potSize;
        } 
        else if (opponentHandValue > playerHandValue) {
            cumulativePlayerWinnings -= (double)this->potSize;
        } else {
            // Equality, do nothing
        }
    }

    averagePlayerWinnings = cumulativePlayerWinnings / (double)AllInSamplesize;


    return averagePlayerWinnings;
}

vector<double> uniformDist(int n)
{
    vector<double> res;
    for (int i = 0; i<n; i++) {
        res.push_back(1.0 / (double)n);
    }

    return res;
}

vector<double> filledArrayDouble(int nbActions, double value)
{
    vector<double> rv;

    for (int i = 0; i< nbActions; i++) {
        rv.push_back(value);
    }
    
    return rv;
}

void PokerNode::instanciate()
{
    // FIX ME: What did I want to do ??? LOL
}

// OOOH MYYYYY GAAAAAAD
void PokerNode::buildChildren()
{
    if (this->isTerminal()) {
        return;
    }

    auto previousAction = this->history.substr(this->history.size() -1);

    if (previousAction == h_RootNode) {
        this->buildRootDeals();
    } else if (previousAction == h_P0Deal) {
        this->buildP1Deal();
    } else if (previousAction == h_P1Deal) {
        this->buildOpenAction();
    } else if (previousAction == h_Chance) {
        this->buildP0Deal();
    } else if (previousAction == h_Check) {
        this->buildCBAction();
    } else if (previousAction == h_Bet1) {
        this->buildCFRAction(true);
    } else if (previousAction == h_Bet2) {

        this->buildCFRAction(true);
    } else if (previousAction == h_Bet3) {

        this->buildCFRAction(true);
    } else if (previousAction == h_Raise1) {

        this->buildCFRAction(true);
    } else if (previousAction == h_Raise2) {

        this->buildCFRAction(true);
    } else if (previousAction == h_AllIn) {

        this->buildCFRAction(false);
    } else if (previousAction == h_CheckBack) {
        if (this->stage == 0 || this->stage == 1) {
   
        this->buildChanceNode();
        } else { return; }
    } else if (previousAction == h_Call) {
        if (this->stage == 0 || this->stage == 1) {

        this->buildChanceNode();
        } else { return; }
    }

/*
    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, 1.0 / (double) nbActions);
    this->ReachPr = 0.0;
    this->ReachPrSum = 0.0;
    // FIX ME: check where this is updated
    this->probabilities = uniformDist(nbActions);
*/

    // FIX ME: Store in map
    // HEY! No Fking map in this program okaaaaaay !!!
}

void PokerNode::buildRootDeals() {
    PokerNode *child = new PokerNode(
        -1, 
        this->limitedRunouts,
        this->potSize,
        this->effectiveSize, 
        this->currentFacingBet,
        this->raiseLevel,
        this->stage,
        this->board,
        this->p0Card,
        this->p1Card,
        h_P0Deal
    );

    this->children.push_back(child);
    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

void PokerNode::buildP0Deal() {
    PokerNode *child = new PokerNode(
        -1, 
        this->limitedRunouts,
        this->potSize, 
        this->effectiveSize,
        this->currentFacingBet,
        this->raiseLevel,
        this->stage,
        this->board, 
        this->p0Card,
        this->p1Card,
        this->history + h_P0Deal
        );
    child->parent = this;
    child->turnIndex = this->turnIndex;

    this->children.push_back(child);
    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

void PokerNode::buildP1Deal() {
    PokerNode *child = new PokerNode(
        0, 
        this->limitedRunouts,
        this->potSize,
        this->effectiveSize, 
        this->currentFacingBet,
        this->raiseLevel,
        this->stage,
        this->board, 
        this->p0Card,
        this->p1Card,
        this->history + h_P1Deal
    );
    child->parent = this;
    child->turnIndex = this->turnIndex;

    this->children.push_back(child);
    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

void PokerNode::buildOpenAction() {

    vector<string> choices = { h_Check };
    vector<double> bets = { 0.0 };

    auto stage = this->stage;

    switch (stage) {
        case 0:
            for (int index = 0; index<OOPFlopBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(OOPFlopBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(OOPFlopBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(OOPFlopBets[index]);
                        break;
                }
            }

        case 1:
            for (int index = 0; index<OOPTurnBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(OOPTurnBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(OOPTurnBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(OOPTurnBets[index]);
                        break;
                }
            }

        case 2:
            for (int index = 0; index<OOPRiverBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(OOPRiverBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(OOPRiverBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(OOPRiverBets[index]);
                        break;
                }
            }
    };

    for (int i = 0; i<choices.size(); i++)
    {
        double addToPotSize = (double)this->potSize * bets[i];

        PokerNode *child = new PokerNode(
            1, 
            this->limitedRunouts,
            this->potSize + (int)addToPotSize,
            this->effectiveSize - (int)addToPotSize, 
            this->currentFacingBet + (int)addToPotSize,
            this->raiseLevel,
            this->stage,
            this->board, 
            this->p0Card,
            this->p1Card,
            this->history + choices[i]
        );
        child->parent = this;
        child->turnIndex = this->turnIndex;

        this->children.push_back(child);
    }

    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

void PokerNode::buildCBAction() {

    vector<string> choices = { h_CheckBack };
    vector<double> bets = { 0.0 };

    auto stage = this->stage;

    switch (stage) {
        case 0:
            for (int index = 0; index<IPFlopBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(IPFlopBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(IPFlopBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(IPFlopBets[index]);
                        break;
                }
            }

        case 1:
            for (int index = 0; index<IPTurnBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(IPTurnBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(IPTurnBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(IPTurnBets[index]);
                        break;
                }
            }

        case 2:
            for (int index = 0; index<IPRiverBets.size(); index++) {
                switch (index) {
                    case 0:
                        choices.push_back(h_Bet1);
                        bets.push_back(IPRiverBets[index]);
                        break;
                    case 1:
                        choices.push_back(h_Bet2);
                        bets.push_back(IPRiverBets[index]);
                        break;
                    case 2:
                        choices.push_back(h_Bet3);
                        bets.push_back(IPRiverBets[index]);
                        break;
                }
            }
    };

    for (int i = 0; i<choices.size(); i++)
    {
        double addToPotSize = (double)this->potSize * bets[i];

        PokerNode *child = new PokerNode(
            0, 
            this->limitedRunouts,
            this->potSize + (int)addToPotSize,
            this->effectiveSize - (int)addToPotSize,
            this->currentFacingBet + (int)addToPotSize,
            this->raiseLevel,
            this->stage,
            this->board, 
            this->p0Card,
            this->p1Card,
            this->history + choices[i]
        );
        child->parent = this;
        child->turnIndex = this->turnIndex;

        this->children.push_back(child);
    }

    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

bool isOverThreasholdBet(const PokerNode *parent, double choice)
{
    return (double)parent->potSize + ((double)parent->potSize * choice) >= (double)parent->effectiveSize * Threashold;
}

bool isOverThreasholdRaise(const PokerNode *parent, double choice)
{
    double potentialRaise = (double)parent->currentFacingBet * choice;
    return potentialRaise + (double)parent->effectiveSize >= (double)parent->effectiveSize*Threashold;
}

void PokerNode::buildCFRAction(bool isRaise)
{
    vector<string> choices = { h_Fold, h_Call };
    vector<double> bets = { 0.0, 0.0 };

    auto stage = this->stage;

    int player;

    if (this->player == 0) {
        player = 1;
    } else {
        player = 0;
    }

    if (isRaise == true)
    {
        if (player == 0)
        {
            switch(stage) {
                case 0:
                    for (int i = 0; i<IPFlopRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, IPFlopRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, IPFlopRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                    }
                break;
                
                case 1:
                     for (int i = 0; i<IPTurnRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, IPTurnRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, IPTurnRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                    }
                    break;

                 case 2:
                     for (int i = 0; i<IPRiverRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, IPRiverRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, IPRiverRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                    
                    }
                    break;
            }
        } // end if player == 0

        else if (player == 1)
        {
            switch(stage) {
                case 0:
                    for (int i = 0; i<OOPFlopRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, OOPFlopRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, OOPFlopRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                    }
                    break;

                case 1:
                     for (int i = 0; i<OOPTurnRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, OOPTurnRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, OOPTurnRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                    }
                    break;

                 case 2:
                     for (int i = 0; i<OOPRiverRaises.size(); i++) {
                        if (i == 1)
                        {
                            if ((isOverThreasholdRaise(this, OOPRiverRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(this, OOPRiverRaises[i]) == true) || this->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(this->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise2);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                    }
                    break;
            }
        }
    } // End ifRaise == true

    for (int index = 0; index<choices.size(); index++)
    {
        double addToPotSize = double(this->potSize) + (bets[index] * this->currentFacingBet); // FIX ME: implement it

        PokerNode *child = new PokerNode(
            player, 
            this->limitedRunouts,
            this->potSize + (int)addToPotSize, 
            this->effectiveSize - (int)addToPotSize,
            this->currentFacingBet + addToPotSize,
            this->raiseLevel,
            this->stage,
            this->board, 
            this->p0Card,
            this->p1Card,
            this->history + choices[index]
        );
        child->parent = this;
        child->turnIndex = this->turnIndex;

        this->children.push_back(child);

    }

    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());
}

void PokerNode::buildChanceNode()
{
    vector<string> validCards;
    int player;

    if (this->player == 0) {
        player = 1;
    } else {
        player = 0;
    }
    
    bool hasTurnIndex = false;
    vector<int> turnIndexes;

    if (this->stage == 0) {
        for (int index = 0; index<this->limitedRunouts.size(); index ++)
        {
            auto card = limitedRunouts[index][0];
            if (find(this->board.begin(), this->board.end(), card) == this->board.end()) {
                validCards.push_back(card);
                turnIndexes.push_back(index);
                hasTurnIndex = true;
            }
        }
    } else if (this->stage == 1) {
        auto card = this->limitedRunouts[this->turnIndex][1];
        validCards.push_back(card);
    }


    for (int index = 0; index<validCards.size(); index++)
    {
        int newNodeStage;
        if (this->stage == 0) { newNodeStage = 1; }
        else if (this->stage == 1) { newNodeStage = 2; }

        int finalTurnIndex = -1;
        if (hasTurnIndex == true) { finalTurnIndex = turnIndexes[index]; }

        auto board = this->board;
        board.push_back(validCards[index]);

        PokerNode *child = new PokerNode(
            this->player, 
            this->limitedRunouts,
            this->potSize,
            this->effectiveSize,
            this->currentFacingBet, 
            this->raiseLevel,
            newNodeStage,
            board, 
            this->p0Card,
            this->p1Card,
            this->history + "*" + validCards[index] + "*" + h_Chance
        );
        child->parent = this;
        child->turnIndex = finalTurnIndex;

        this->children.push_back(child);
    }



    int nbActions = this->children.size();
    this->RegretSum = filledArrayDouble(nbActions, 0.0);
    this->StrategySum = filledArrayDouble(nbActions, 0.0);
    this->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    this->probabilities = uniformDist(this->children.size());


}