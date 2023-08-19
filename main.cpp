#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <stdio.h>

#include "Deck.hpp"
#include "Handsolver.hpp"
#include "Poker.hpp"
#include "Range.hpp"
#include "Cfr.hpp"

#include "backward.hpp"


using namespace std;
using namespace std::chrono;

const int N_ITERATIONS = 100;

int main()
{
    Deck deck;
    Handsolver handsolver;
    Range range;

    auto start = high_resolution_clock::now();

    auto lro = getLimitedRunouts(100);

    auto handsOOP = range.range_to_list(MatrixOOP);
    auto handsIP = range.range_to_list(MatrixIp);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(std::begin(handsOOP), std::end(handsOOP), e);
    std::shuffle(std::begin(handsIP), std::end(handsIP), e);

    auto cfr = CFR();

    double expectedValue = 0.0;

    PokerNode *root = new PokerNode(-1, lro, Pot, EffectiveStack, 0, 0, 0, board, handsOOP[0], handsIP[0], h_RootNode);
        // Have to take this outside this with a pre root of something...

    for (int i = 0; i<N_ITERATIONS; i++)
    {
        if (i % 10 == 0) {
            cout << "iter : " << i << endl;
        }

        // Generate root node
        std::shuffle(std::begin(handsOOP), std::end(handsOOP), e);
        std::shuffle(std::begin(handsIP), std::end(handsIP), e);

        Hand validHandOOP;
        Hand validHandIP;
        vector<string> currentForbiddenCards = board;

        for (int i = 0; i<handsOOP.size(); i++)
        {
            if ((find(board.begin(), board.end(), handsOOP[i].Cards[0]) == board.end()) &&  (find(board.begin(), board.end(), handsOOP[i].Cards[1]) == board.end()))
            {
                validHandOOP = handsOOP[i];
                currentForbiddenCards.push_back(handsOOP[i].Cards[0]);
                currentForbiddenCards.push_back(handsOOP[i].Cards[1]);
                break;
            }
        }

        for (int i = 0; i<handsIP.size(); i++)
        {
            if ((find(currentForbiddenCards.begin(), currentForbiddenCards.end(), handsIP[i].Cards[0]) == currentForbiddenCards.end()) &&  (find(currentForbiddenCards.begin(), currentForbiddenCards.end(), handsIP[i].Cards[1]) == currentForbiddenCards.end()))
            {
                validHandIP = handsIP[i];
                break;
            }
        }

        root->p0Card = validHandOOP;
        root->p1Card = validHandIP;

        // printf("Check root node\neffecitve size : %d, pot size : %d\n", root->effectiveSize, root->potSize);

        cfr.run(root);
        // try {
        //     printf("Root cards\n p0 : %s %s\np1 : %s %s\n", root->p0Card.Cards.at(0).c_str(), root->p0Card.Cards.at(1).c_str(), root->p1Card.Cards.at(0).c_str(), root->p1Card.Cards.at(0).c_str());
        // }
        // catch(const std::exception& e) {
        //     cout << e.what() << endl;
        //     cout << "Exit because error" << endl;
        //     break;
        // }

        // Update Tree now


    }











    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << duration.count() << endl;

    

    return 0;
}