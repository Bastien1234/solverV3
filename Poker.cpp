// Once node is created, push it onto the map directly to avoid recreation and only one map for everything 

#include "Poker.hpp"

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <stdio.h> // DEBUG

#include "Hand.hpp"
#include "Handsolver.hpp"
#include "Deck.hpp"
#include "NodeStrategy.hpp"
#include "backward.hpp"
#include "Logs.hpp"

using namespace std;

unordered_map<string, long> memoMap;
Handsolver handsolver;
Deck deckMaker;

long getMemoValue(vector<string> array)
{
    text_log(LOGS_ENABLED, "getMemoValue-in");
    auto sorted = array;
    sort(begin(sorted), end(sorted));
    string final = "";
    for (auto el : sorted)
    {
        final += el;
    }

    if (memoMap.find(final) != memoMap.end())
    {
        text_log(LOGS_ENABLED, "getMemoValue-out");
        return memoMap[final];
    }
    else 
    {
        memoMap[final] = handsolver.solve(array);
        text_log(LOGS_ENABLED, "getMemoValue-out");
        return memoMap[final];
    }
}

vector<vector<string>> getLimitedRunouts(int nbRunouts)
{
    text_log(LOGS_ENABLED, "getLimitedRunouts-in");
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
            text_log(LOGS_ENABLED, "getLimitedRunouts-out");
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
    // this->turnIndex = -1;
};

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
        string _history,
        int _turnIndex)
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
        history(_history),
        turnIndex(_turnIndex)
{
    // std::cout << "built with turn index : " << _turnIndex << std::endl;
};

PokerNode::~PokerNode(){};


int PokerNode::getPlayer()
{
    return this->player;
}

int numChildren(PokerNode *pokerNode, MasterMap* masterMap)
{
    if (pokerNode->childrenHistory.size() == 0)
    {
        buildChildren(pokerNode, masterMap);
    }

    return pokerNode->childrenHistory.size();
}

PokerNode getChild(PokerNode *pokerNode, int i, MasterMap* masterMap)
{
    text_log(LOGS_ENABLED, "getChild-in");
    // FIX ME: Get from mastermap with cards etc... 
    if (pokerNode->childrenHistory.size() == 0)
    {
        buildChildren(pokerNode, masterMap);
    }


    std::string index = pokerNode->childrenHistory[i];

    auto val = masterMap->getNode(index);

    text_log(LOGS_ENABLED, "getChild-out");
    return val;
}

PokerNode PokerNode::getParent()
{
    // FIX ME: maybe remote it
    // return this.parent;
}

double getChildProbability(PokerNode *pokerNode, int i, MasterMap *masterMap)
{
    text_log(LOGS_ENABLED, "getChildProbability-in");
    if (pokerNode->probabilities.size() == 0)
    {
        buildChildren(pokerNode, masterMap);
    }

    text_log(LOGS_ENABLED, "getChildProbability-out");
    return pokerNode->probabilities[i];
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
    text_log(LOGS_ENABLED, "getFullBoard-in");
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

    text_log(LOGS_ENABLED, "getFullBoard-out");
    return fullBoard;
}

double PokerNode::utility(int player)
{
    text_log(LOGS_ENABLED, "PokerNode::utility-in");
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
            text_log(LOGS_ENABLED, "PokerNode::utility-out");
            return (double)this->potSize;
        } else {
            text_log(LOGS_ENABLED, "PokerNode::utility-out");
            return -(double)this->potSize;
        }
    }

    // Case river showdown
    if (this->stage == 2) {;

        vector<string> playerFinalHand = board;
        playerFinalHand.push_back(cardPlayer.Cards.at(0));
        playerFinalHand.push_back(cardPlayer.Cards.at(1));


        vector<string> opponentFinalhand = board;
        opponentFinalhand.push_back(cardOpponent.Cards.at(0));
        opponentFinalhand.push_back(cardOpponent.Cards.at(1));

        string handhero = "";
        for (auto el : cardPlayer.Cards) { handhero.append(el); }

        string handvilain = "";
        for (auto el : cardOpponent.Cards) { handvilain.append(el); }

        long playerHandValue = handsolver.solve(playerFinalHand);
        long opponentHandValue = handsolver.solve(opponentFinalhand);

        if (playerHandValue > opponentHandValue) {
            text_log(LOGS_ENABLED, "PokerNode::utility-out");
            return (double)this->potSize;
        } 
        else if (opponentHandValue > playerHandValue) {
            text_log(LOGS_ENABLED, "PokerNode::utility-out");
            return - (double)this->potSize;
        } else {
            text_log(LOGS_ENABLED, "PokerNode::utility-out");
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

    text_log(LOGS_ENABLED, "PokerNode::utility-out");
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
std::vector<PokerNode> buildChildren(PokerNode* pokerNode, MasterMap* masterMap)
{
    text_log(LOGS_ENABLED, "buildChildren-in");
    if (pokerNode->isTerminal()) {
        std::cout << "SHOULD NEVER BE HERE" << std::endl;
        return std::vector<PokerNode> {};
    }

    auto previousAction = pokerNode->history.substr(pokerNode->history.size() -1);

    if (previousAction == h_RootNode) {
        return buildRootDeals(pokerNode, masterMap);
    } else if (previousAction == h_P0Deal) {
        return buildP1Deal(pokerNode, masterMap);
    } else if (previousAction == h_P1Deal) {
        return buildOpenAction(pokerNode, masterMap);
    } else if (previousAction == h_Chance) {
        return buildP0Deal(pokerNode, masterMap);
    } else if (previousAction == h_Check) {
        return buildCBAction(pokerNode, masterMap);
    } else if (previousAction == h_Bet1) {
        return buildCFRAction(pokerNode, masterMap, true);
    } else if (previousAction == h_Bet2) {
        return buildCFRAction(pokerNode, masterMap, true);
    } else if (previousAction == h_Bet3) {
        return buildCFRAction(pokerNode, masterMap, true);
    } else if (previousAction == h_Raise1) {
        return buildCFRAction(pokerNode, masterMap, true);
    } else if (previousAction == h_Raise2) {
        return buildCFRAction(pokerNode, masterMap, true);
    } else if (previousAction == h_AllIn) {
        return buildCFRAction(pokerNode, masterMap, false);
    } else if (previousAction == h_CheckBack) {
        if (pokerNode->stage == 0 || pokerNode->stage == 1) {
            return buildChanceNode(pokerNode, masterMap);
        } else { 
            std::cout << "SHOULD NEVER BE HERE" << std::endl;
            return std::vector<PokerNode> {}; 
        }
    } else if (previousAction == h_Call) {
        if (pokerNode->stage == 0 || pokerNode->stage == 1) {
            return buildChanceNode(pokerNode, masterMap);
        } else { 
            std::cout << "SHOULD NEVER BE HERE" << std::endl;
            return std::vector<PokerNode> {}; 
        }
    }
    text_log(LOGS_ENABLED, "buildChildren-out");
}

std::vector<PokerNode> buildRootDeals(PokerNode* pokerNode, MasterMap* masterMap) {
    text_log(LOGS_ENABLED, "buildRootDeals-in");
    auto currPlayer = pokerNode->getPlayer();
    std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + h_P0Deal));
    PokerNode child = PokerNode(
        -1, 
        pokerNode->limitedRunouts,
        pokerNode->potSize,
        pokerNode->effectiveSize, 
        pokerNode->currentFacingBet,
        pokerNode->raiseLevel,
        pokerNode->stage,
        pokerNode->board,
        pokerNode->p0Card,
        pokerNode->p1Card,
        nextHistory
    );
    // FIX ME: memoize the following code, maybe with a 'computed' variable ?

// CODE children history instead of children
    // pokerNode->children.push_back(child);
    pokerNode->childrenHistory.push_back(nextHistory);
    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->childrenHistory.size());

    masterMap->add(std::vector<PokerNode>{child});

    text_log(LOGS_ENABLED, "buildRootDeals-out");
    return vector<PokerNode>{child};
}

std::vector<PokerNode> buildP0Deal(PokerNode* pokerNode, MasterMap* masterMap) {
    text_log(LOGS_ENABLED, "buildP0Deal-in");
    auto currPlayer = pokerNode->getPlayer();
    std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + h_P0Deal));
    PokerNode child = PokerNode(
        -1, 
        pokerNode->limitedRunouts,
        pokerNode->potSize, 
        pokerNode->effectiveSize,
        pokerNode->currentFacingBet,
        pokerNode->raiseLevel,
        pokerNode->stage,
        pokerNode->board, 
        pokerNode->p0Card,
        pokerNode->p1Card,
        nextHistory
        );
   // child.parent = this;
    child.turnIndex = pokerNode->turnIndex;

    // pokerNode->children.push_back(child);
    pokerNode->childrenHistory.push_back(nextHistory);
    int nbActions = pokerNode->childrenHistory.size();
    // int nbActions = pokerNode->children.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->children.size());

    masterMap->add(std::vector<PokerNode>{child});

    text_log(LOGS_ENABLED, "buildP0Deal-out");
    return vector<PokerNode>{child};
}

std::vector<PokerNode> buildP1Deal(PokerNode* pokerNode, MasterMap* masterMap) {
    text_log(LOGS_ENABLED, "buildP1Deal-in");
    auto currPlayer = pokerNode->getPlayer();
    std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + h_P1Deal));
    PokerNode child = PokerNode(
        0, 
        pokerNode->limitedRunouts,
        pokerNode->potSize,
        pokerNode->effectiveSize, 
        pokerNode->currentFacingBet,
        pokerNode->raiseLevel,
        pokerNode->stage,
        pokerNode->board, 
        pokerNode->p0Card,
        pokerNode->p1Card,
        nextHistory
    );
    // child.parent = this;
    child.turnIndex = pokerNode->turnIndex;

    // pokerNode->children.push_back(child);
    pokerNode->childrenHistory.push_back(nextHistory);
    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->children.size());


    masterMap->add(std::vector<PokerNode>{child});

    text_log(LOGS_ENABLED, "buildP1Deal-out");
    return vector<PokerNode>{child};
}

std::vector<PokerNode> buildOpenAction(PokerNode* pokerNode, MasterMap* masterMap) {
    text_log(LOGS_ENABLED, "buildOpenAction-in");
    vector<string> choices = { h_Check };
    vector<double> bets = { 0.0 };

    auto stage = pokerNode->stage;

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

    vector<PokerNode> vectorToReturn;

    for (int i = 0; i<choices.size(); i++)
    {
        double addToPotSize = (double)pokerNode->potSize * bets[i];
        auto currPlayer = pokerNode->getPlayer();
        std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + choices[i]));
        PokerNode child = PokerNode(
            1, 
            pokerNode->limitedRunouts,
            pokerNode->potSize + (int)addToPotSize,
            pokerNode->effectiveSize - (int)addToPotSize, 
            pokerNode->currentFacingBet + (int)addToPotSize,
            pokerNode->raiseLevel,
            pokerNode->stage,
            pokerNode->board, 
            pokerNode->p0Card,
            pokerNode->p1Card,
            nextHistory
        );
        // child.parent = this;
        child.turnIndex = pokerNode->turnIndex;

        pokerNode->childrenHistory.push_back(nextHistory);
        vectorToReturn.push_back(child);
    }

    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->childrenHistory.size());

    masterMap->add(vectorToReturn);

    text_log(LOGS_ENABLED, "buildOpenAction-out");
    return vectorToReturn;
}

std::vector<PokerNode> buildCBAction(PokerNode* pokerNode, MasterMap* masterMap) {
    text_log(LOGS_ENABLED, "buildCBAction-in");

    vector<string> choices = { h_CheckBack };
    vector<double> bets = { 0.0 };

    auto stage = pokerNode->stage;

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

    vector<PokerNode> vectorToReturn;

    for (int i = 0; i<choices.size(); i++)
    {
        double addToPotSize = (double)pokerNode->potSize * bets[i];
        auto currPlayer = pokerNode->getPlayer();
        std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + choices[i]));
        PokerNode child = PokerNode(
            0, 
            pokerNode->limitedRunouts,
            pokerNode->potSize + (int)addToPotSize,
            pokerNode->effectiveSize - (int)addToPotSize,
            pokerNode->currentFacingBet + (int)addToPotSize,
            pokerNode->raiseLevel,
            pokerNode->stage,
            pokerNode->board, 
            pokerNode->p0Card,
            pokerNode->p1Card,
            nextHistory
        );
        // child.parent = this;
        child.turnIndex = pokerNode->turnIndex;

        // pokerNode->children.push_back(child);
        pokerNode->childrenHistory.push_back(nextHistory);
        vectorToReturn.push_back(child);
    }

    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->children.size());

    masterMap->add(vectorToReturn);

    text_log(LOGS_ENABLED, "buildCBAction-out");
    return vectorToReturn;
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

std::vector<PokerNode> buildCFRAction(PokerNode *pokerNode, MasterMap* masterMap, bool isRaise)
{
    text_log(LOGS_ENABLED, "buildCFRAction-in");
    vector<string> choices = { h_Fold, h_Call };
    vector<double> bets = { 0.0, 0.0 };

    auto stage = pokerNode->stage;

    int player;

    if (pokerNode->player == 0) {
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
                            if ((isOverThreasholdRaise(pokerNode, IPFlopRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, IPFlopRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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
                            if ((isOverThreasholdRaise(pokerNode, IPTurnRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, IPTurnRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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
                            if ((isOverThreasholdRaise(pokerNode, IPRiverRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(IPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, IPRiverRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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
                            if ((isOverThreasholdRaise(pokerNode, OOPFlopRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, OOPFlopRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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
                            if ((isOverThreasholdRaise(pokerNode, OOPTurnRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, OOPTurnRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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
                            if ((isOverThreasholdRaise(pokerNode, OOPRiverRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
                                break;
                            } else {
                                choices.push_back(h_Raise1);
                                bets.push_back(OOPFlopBets[i]);
                            }
                        }
                        else if (i == 2)
                        {
                            if ((isOverThreasholdRaise(pokerNode, OOPRiverRaises[i]) == true) || pokerNode->raiseLevel == MaxRaises )
                            {
                                choices.push_back(h_AllIn);
                                bets.push_back(double(pokerNode->effectiveSize));
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

    vector<PokerNode> vectorToReturn;

    for (int index = 0; index<choices.size(); index++)
    {
        double addToPotSize = double(pokerNode->potSize) + (bets[index] * pokerNode->currentFacingBet);
        auto currPlayer = pokerNode->getPlayer();
        std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + choices[index]));
        PokerNode child = PokerNode(
            player, 
            pokerNode->limitedRunouts,
            pokerNode->potSize + (int)addToPotSize, 
            pokerNode->effectiveSize - (int)addToPotSize,
            pokerNode->currentFacingBet + addToPotSize,
            pokerNode->raiseLevel,
            pokerNode->stage,
            pokerNode->board, 
            pokerNode->p0Card,
            pokerNode->p1Card,
            nextHistory
        );
        // child.parent = this;
        child.turnIndex = pokerNode->turnIndex;

        //pokerNode->children.push_back(child);
        pokerNode->childrenHistory.push_back(nextHistory);
        vectorToReturn.push_back(child);
    }
    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->childrenHistory.size());
    
    masterMap->add(vectorToReturn);

    text_log(LOGS_ENABLED, "buildCFRAction-out");
    return vectorToReturn;
}

/*
std::vector<PokerNode> buildChanceNode(PokerNode *pokerNode, MasterMap* masterMap)
{
    vector<string> validCards;
    int player;

    if (pokerNode->player == 0) {
        player = 1;
    } else {
        player = 0;
    }
    
    bool hasTurnIndex = false;
    vector<int> turnIndexes;

    if (pokerNode->stage == 0) {
        for (int index = 0; index<pokerNode->limitedRunouts.size(); index ++)
        {
            auto card = pokerNode->limitedRunouts[index][0];
            if (find(pokerNode->board.begin(), pokerNode->board.end(), card) == pokerNode->board.end()) {
                validCards.push_back(card);
                turnIndexes.push_back(index);
                hasTurnIndex = true;
            }
        }
    } else if (pokerNode->stage == 1) {
        auto card = pokerNode->limitedRunouts[pokerNode->turnIndex][1];
        validCards.push_back(card);
    }

    vector<PokerNode> vectorToReturn;


    for (int index = 0; index<validCards.size(); index++)
    {
        int newNodeStage;
        if (pokerNode->stage == 0) { newNodeStage = 1; }
        else if (pokerNode->stage == 1) { newNodeStage = 2; }

        int finalTurnIndex = -1;
        if (hasTurnIndex == true) { finalTurnIndex = turnIndexes[index]; }

        auto board = pokerNode->board;
        board.push_back(validCards[index]);
        auto currPlayer = pokerNode->getPlayer();
        std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + "*" + validCards[index] + "*" + h_Chance));    

        PokerNode child =PokerNode(
            pokerNode->player, 
            pokerNode->limitedRunouts,
            pokerNode->potSize,
            pokerNode->effectiveSize,
            pokerNode->currentFacingBet, 
            pokerNode->raiseLevel,
            newNodeStage,
            board, 
            pokerNode->p0Card,
            pokerNode->p1Card,
            nextHistory
        );
        // child.parent = this;
        child.turnIndex = finalTurnIndex;

        // pokerNode->children.push_back(child);
        pokerNode->childrenHistory.push_back(nextHistory);
        vectorToReturn.push_back(child);
    }



    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->childrenHistory.size());

    masterMap->add(vectorToReturn);

    return vectorToReturn;


}
*/

// In this version we ignore limited run outs and give back one node only
std::vector<PokerNode> buildChanceNode(PokerNode *pokerNode, MasterMap* masterMap)
{
    text_log(LOGS_ENABLED, "buildChanceNode-in");
    vector<string> validCards;
    int player;

    if (pokerNode->player == 0) {
        player = 1;
    } else {
        player = 0;
    }

    text_log(LOGS_ENABLED, "buildChanceNode-check event before huh");

    // ---------------------------

    Deck deckMaker;

    auto deck = deckMaker.make_deck();

    std::vector<std::string> forbiddenCards;

    for (int i=0; i<2; i++)
    {
        forbiddenCards.push_back(pokerNode->p0Card.Cards[i]);
        forbiddenCards.push_back(pokerNode->p1Card.Cards[i]);
    }

    for (auto el : board)
    {
        forbiddenCards.push_back(el);
    }

    string cardToPush;

    for (auto card : deck)
    {
        if (std::find(forbiddenCards.begin(), forbiddenCards.end(), card) != forbiddenCards.end())
        {
            continue;
        }
        else
        {
            cardToPush = card;
            break;
        }
    }

    vector<PokerNode> vectorToReturn;
    text_log(LOGS_ENABLED, "buildChanceNode-1");


    int newNodeStage;
    if (pokerNode->stage == 0) { newNodeStage = 1; }
    else if (pokerNode->stage == 1) { newNodeStage = 2; }

    auto board = pokerNode->board;
    board.push_back(cardToPush);
    auto currPlayer = pokerNode->getPlayer();
    std::string nextHistory = pokerNode->computeCardHistory(currPlayer, (pokerNode->history + "*" + cardToPush + "*" + h_Chance));   

    // If the history already exists, return it
    if (masterMap->map.contains(nextHistory))
    {
        auto node = masterMap->getNode(nextHistory);
        vectorToReturn.push_back(node);
        return vectorToReturn;
    }


    text_log(LOGS_ENABLED, "buildChanceNode-2");


    PokerNode child = PokerNode(
        pokerNode->player, 
        pokerNode->limitedRunouts,
        pokerNode->potSize,
        pokerNode->effectiveSize,
        pokerNode->currentFacingBet, 
        pokerNode->raiseLevel,
        newNodeStage,
        board, 
        pokerNode->p0Card,
        pokerNode->p1Card,
        nextHistory
    );
    // child.parent = this;

    // pokerNode->children.push_back(child);
    pokerNode->childrenHistory.push_back(nextHistory);
    vectorToReturn.push_back(child);
    

    text_log(LOGS_ENABLED, "buildChanceNode-3");



    int nbActions = pokerNode->childrenHistory.size();
    pokerNode->RegretSum = filledArrayDouble(nbActions, 0.0);
    pokerNode->StrategySum = filledArrayDouble(nbActions, 0.0);
    pokerNode->Strategy = filledArrayDouble(nbActions, (1.0 / (float)nbActions));
    pokerNode->probabilities = uniformDist(pokerNode->childrenHistory.size());

    masterMap->add(vectorToReturn);

    text_log(LOGS_ENABLED, "buildChanceNode-out");
    return vectorToReturn;


}

std::string PokerNode::computeCardHistory(int currentPlayer, std::string history)
{
    text_log(LOGS_ENABLED, "computeCardHistory-in");
    std::string returnVal = history;
    std::string nextCards;
    if (currentPlayer == 1)
    {
        auto card1 = this->p0Card.Cards[0];
        auto card2 = this->p0Card.Cards[1];

        nextCards = card1 + card2;
    }
    else 
    {
        auto card1 = this->p1Card.Cards[0];
        auto card2 = this->p1Card.Cards[1];

        nextCards = card1 + card2;
    }

    for (int i=0; i<4; i++)
    {
        returnVal[i] = nextCards[i];
    }

    text_log(LOGS_ENABLED, "computeCardHistory-out");
    return returnVal;
}

MasterMap::MasterMap()
{
    // Pre allocate some size ?
}

MasterMap::~MasterMap()
{
    for (auto n : this->map)
    {
        delete(n.second);
    }
}

void MasterMap::add(std::vector<PokerNode> children)
{
    text_log(LOGS_ENABLED, "MasterMap::add-in");
    for (auto child : children)
    {
        // int currentPlayer = child.getPlayer();
        
        PokerNode *heapNode = new PokerNode(
            child.player, 
            child.limitedRunouts,
            child.potSize,
            child.effectiveSize,
            child.currentFacingBet, 
            child.raiseLevel,
            child.stage,
            child.board, 
            child.p0Card,
            child.p1Card,
            child.history,
            child.turnIndex
        );

        heapNode->ReachPr = child.ReachPr;
        heapNode->ReachPrSum = child.ReachPrSum;
        heapNode->RegretSum = child.RegretSum;
        heapNode->Strategy = child.Strategy;
        heapNode->StrategySum = child.StrategySum;
        heapNode->probabilities = child.probabilities;

        this->map[child.history] = heapNode;
    };
    text_log(LOGS_ENABLED, "MasterMap::add-out");
}

PokerNode MasterMap::getNode(std::string str)
{

    // FIX ME: frequencies !!!
    auto val = this->map[str];
    auto node = PokerNode(
            val->player, 
            val->limitedRunouts,
            val->potSize,
            val->effectiveSize,
            val->currentFacingBet, 
            val->raiseLevel,
            val->stage,
            val->board, 
            val->p0Card,
            val->p1Card,
            val->history
    );

    node.ReachPr = val->ReachPr;
    node.ReachPrSum = val->ReachPrSum;
    node.RegretSum = val->RegretSum;
    node.Strategy = val->Strategy;
    node.StrategySum = val->StrategySum;
    node.probabilities = val->probabilities;

    return node;
}

void MasterMap::Update()
{
    text_log(LOGS_ENABLED, "MasterMap::Update()-in");

    for (auto kv : this->map)
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

    text_log(LOGS_ENABLED, "MasterMap::Update()-out");
}