#include "NodeStrategy.hpp"
#include "Utils.hpp"

#include <vector>

NodeStrategy::NodeStrategy(int nbActions)
{
    RegretSum = FilledArrayDouble(nbActions, 0.0);
    StrategySum = FilledArrayDouble(nbActions, 0.0);
    Strategy = FilledArrayDouble(nbActions, 1.0/(double)nbActions);

    ReachPr = 0.0;
    ReachPrSum = 0.0;

    Visited = 0;

    // FIX ME: Remove it maybe
}