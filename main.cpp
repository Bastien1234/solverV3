#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <stdio.h>
#include <queue>
#include <fstream>

#include "Deck.hpp"
#include "Handsolver.hpp"
#include "Poker.hpp"
#include "Range.hpp"
#include "Cfr.hpp"
#include "Logs.hpp"

#include "Test.hpp"

#include "backward.hpp"

// #include <nlohmann/json.hpp>

// using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

const int N_ITERATIONS = 100; // FIX ME: move to constants ?



int main(int argc, char** argv)
{

    for (int i = 0; i < argc; ++i)
    {
        string ar(argv[i]);
        if (ar == "test")
        {
            RunAllTests();

            return 0;
        }
    }
    
    Deck deck;
    Handsolver handsolver;
    Range range;

    auto start = high_resolution_clock::now();

    auto lro = getLimitedRunouts(50);

    auto _handsOOP = range.range_to_list(MatrixOOP);
    auto _handsIP = range.range_to_list(MatrixIp);
    auto handsOOP = vector<Hand>(_handsOOP.begin(), _handsOOP.begin() + HandsToKeepFromRange);
    auto handsIP = vector<Hand>(_handsIP.begin(), _handsIP.begin() + HandsToKeepFromRange);


    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    /*
    std::shuffle(std::begin(handsOOP), std::end(handsOOP), e);
    std::shuffle(std::begin(handsIP), std::end(handsIP), e);
    PokerNode root = PokerNode(-1, lro, Pot, EffectiveStack, 0, 0, 0, board, handsOOP[0], handsIP[0], h_RootNode);
    */

    auto cfr = CFR();
    auto masterMap = MasterMap();

    double expectedValue = 0.0;

    for (int i = 0; i<N_ITERATIONS; i++)
    {
        if (i % 10 == 0) {
            cout << "iter : " << i << endl;
        }

        text_log(LOGS_ENABLED, "ITER");
        

        // Generate root node
        PokerNode root = PokerNode(-1, lro, Pot, EffectiveStack, 0, 0, 0, board, handsOOP[0], handsIP[0], (handsOOP[0].Cards[0] + handsOOP[0].Cards[1] + h_RootNode));
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

        root.p0Card = validHandOOP;
        root.p1Card = validHandIP;

        masterMap.add(std::vector<PokerNode>{root});


        // printf("Check root node\neffecitve size : %d, pot size : %d\n", root->effectiveSize, root->potSize);

        cfr.run(root, &masterMap);

        masterMap.Update();

    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << duration.count() << endl;

    printf("Saving results\n");
    int MAX_RESULTS = 100;
    int total_results = 0;

    ofstream myFile;
    myFile.open("logs.txt");

    for (auto el : masterMap.map)
    {
        std::cout << "history : " << el.first << " value : " << endl;
        for (auto elll : el.second->Strategy)
        {
            std::cout << elll;
        }

        std::cout << "\n";

        total_results++;

        if (total_results >= MAX_RESULTS) { break; }
    }

    return 0;
}