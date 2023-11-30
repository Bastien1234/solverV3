#include "Test.hpp"

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <iostream>

#include "Handsolver.hpp"

void testHandSolver(vector<string> arr, long val)
{
    Handsolver hs;

    bool scs = hs.solve(arr) == val;
    if (scs == false)
    {
        printf("TEST FAILED! for\n");
        string str = "";
        for (auto s : arr) { str.append(s); }
        std::cout << str << " = " << val << std::endl;
    }
}

void RunAllTests()
{
    printf("=== Start testing ===\n");

    testHandSolver(vector<string>{"Ad", "Kd", "Qd", "Jd", "Td", "3h", "2h"}, 914000000000);
    testHandSolver(vector<string>{"As", "Kd", "Qd", "Jd", "9d", "3h", "2h"}, 914000000000);

    

    printf("=== Finished testing ===\n");

}

