#include "Handsolver.hpp"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>


Handsolver::Handsolver()
{
}

Handsolver::~Handsolver()
{
}

long Handsolver::solve(std::vector<std::string> arr)
{
    printf("Entering solve function\n");

    std::string curhand = "";
    for (auto e : arr) { curhand.append(e); }
    std::cout << "current hand = " << curhand << std::endl;
    // Declare state as bits
    unsigned short int stateStraight = 0;
    unsigned short int stateHearts = 0;
    unsigned short int stateDiamonds = 0;
    unsigned short int stateClubs = 0;
    unsigned short int stateSpades = 0;

    // Other states
    int fourOfAKind = -1;
    int threeOfAKind = -1;
    int bestPair = -1;
    int secondPair = -1;

    bool possibleColor = false;
    string stateColor = "";

    int straightValue = -1;

    int colorsCount[] = { 0, 0, 0, 0 };
    int valuesCount[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    printf("Check point 1\n");

    for (auto element : arr)
    {
        string card = element.substr(0, 1);
        string color = element.substr(1, 2);

        if (color ==  "h")
        {
			colorsCount[0] ++;
			stateHearts |= 1 << cardsValueMap[card];
			if (card == "A") {
				stateHearts |= 1 << 1;
			}
        }

		else if (color ==  "d") 
        {
			colorsCount[1] += 1;
			stateDiamonds |= 1 << cardsValueMap[card];
			if (card == "A") {
				stateDiamonds |= 1 << 1;
			}
        }

		else if (color ==  "c")
        {
			colorsCount[2] += 1;
			stateClubs |= 1 << cardsValueMap[card];
			if (card == "A") {
				stateClubs |= 1 << 1;
			}
        }

		else if (color ==  "s")
        {
			colorsCount[3] += 1;
			stateSpades |= 1 << cardsValueMap[card];
			if (card == "A") {
				stateSpades |= 1 << 1;
			}
        }

		stateStraight |= 1 << cardsValueMap[card];
		if (card == "A") {
			stateStraight |= 1 << 1;
		}

		valuesCount[cardsValueMap[card]-1] += 1;
		if (card == "A") {
			valuesCount[0] += 1;
		}
    }

    for (int i=0; i<4; i++)
    {
        if (colorsCount[i] >= 5)
        {
            possibleColor = true;

            if (i == 0)
            {
                stateColor = "h";
                break;
            }

            else if (i == 1)
            {
                stateColor = "d";
                break;
            }

            else if (i == 2)
            {
                stateColor = "c";
                break;
            }

            else if (i == 3)
            {
                stateColor = "s";
                break;
            }
        }
    }

    printf("Check point 2\n");


    for (int i=0; i<14; i++)
    {
        if (valuesCount[i] == 4)
        {
            fourOfAKind = i+1;
            break;
        }

        if (valuesCount[i] == 3)
        {
            if (threeOfAKind != -1)
            {
				if (valuesCount[i] > threeOfAKind) {
					threeOfAKind = i+1;
				}
			}
            else
            {
				threeOfAKind = i+1;
			}
        }

        if (valuesCount[i] == 2)
        {
            // If no current pair
			if (bestPair == -1 && secondPair == -1)
            {
				bestPair = i+1;
			}
            else if (bestPair != -1 && secondPair == -1)
            {
				auto currentBest = bestPair;
				if (valuesCount[i] > currentBest)
                {
					bestPair = i+1;
					secondPair = currentBest;
				}
                else
                {
					secondPair = i+1;
				}
			}

            else
            {

				// If we already have two pairs
				auto currentBest = bestPair;
				auto currentSecondBest = secondPair;

				// Candidate pair is the best
				if (valuesCount[i] > currentBest)
                {
					bestPair = i+1;
					secondPair = currentBest;
				}
                else if (valuesCount[i] > currentSecondBest)
                {
					secondPair = i+1;
				}

			}
        }
    }

    // Check if possible straight
    for (const auto& [value, bitMask] : bitMaskStraightValues)
    {
        if ((stateStraight & bitMask) == bitMask) {
			if (value > straightValue) {
				straightValue = value;
			}
		}
    }

    printf("Check point 3\n");



    /*
	   Check if possible straight flush
	   Return 9 billions then value of high card
	*/

    if ((possibleColor == true) && (straightValue > 0))
    {
        int height = -1;

        if (stateColor == "h")
        {
             for (const auto& [value, bitMask] : bitMaskStraightValues)
            {
                if ((stateHearts & bitMask) == bitMask) {
                    if (value > height) {
                        height = value;
                    }
                }
            }
        }

        else if (stateColor == "d")
        {
             for (const auto& [value, bitMask] : bitMaskStraightValues)
            {
                if ((stateDiamonds & bitMask) == bitMask) {
                    if (value > height) {
                        height = value;
                    }
                }
            }
        }

        else if (stateColor == "c")
        {
             for (const auto& [value, bitMask] : bitMaskStraightValues)
            {
                if ((stateClubs & bitMask) == bitMask) {
                    if (value > height) {
                        height = value;
                    }
                }
            }
        }

        else if (stateColor == "s")
        {
             for (const auto& [value, bitMask] : bitMaskStraightValues)
            {
                if ((stateSpades & bitMask) == bitMask) {
                    if (value > height) {
                        height = value;
                    }
                }
            }
        }

        if (height > 0)
        {
            return 900000000000 + (height * 1e9);
        }
    }

    /*
	   Check is possible four of a kind
	   returns 8 billions then the FOAK value and finally the kicker
	*/

	if (fourOfAKind > 0) {
		for (int index = 14; index > 0; index--) {
			if (((stateStraight&(1<<index)) != 0) && (fourOfAKind != index))
            {
				return 800000000000 + (fourOfAKind * 1e9) + (index * 1e7);
			}
		}
	}

    /*
	   Check is possible full house
	*/

    printf("Check point 4\n");


	if (threeOfAKind > 0 && bestPair > 0)
    {
		return 700000000000 + (threeOfAKind * 1e9) + (bestPair * 1e7);
	}

    if (possibleColor == true)
    {
        vector<int> values;
        int totalValues = 0;

        if (stateColor == "h")
        {
            for (int index = 14; index > 0; index--)
            {
				if ((stateHearts&(1<<index)) != 0)
                {
					values.push_back(index);
					totalValues ++;
					if (totalValues == 5)
                    {
						break;
					}
				}
			}
        }

        else if (stateColor == "d")
        {
            for (int index = 14; index > 0; index--)
            {
				if ((stateDiamonds&(1<<index)) != 0)
                {
					values.push_back(index);
					totalValues ++;
					if (totalValues == 5)
                    {
						break;
					}
				}
			}
        }

        else if (stateColor == "c")
        {
            for (int index = 14; index > 0; index--)
            {
				if ((stateClubs&(1<<index)) != 0)
                {
					values.push_back(index);
					totalValues ++;
					if (totalValues == 5)
                    {
						break;
					}
				}
			}
        }

        else if (stateColor == "s")
        {
            for (int index = 14; index > 0; index--)
            {
				if ((stateSpades&(1<<index)) != 0)
                {
					values.push_back(index);
					totalValues ++;
					if (totalValues == 5)
                    {
						break;
					}
				}
			}
        }

        long multiplier = 1e9;
        long returnValue = 600000000000;

        for (int i=0; i<5; i++)
        {
            returnValue += (values.at(i) * multiplier);
            multiplier /= 100;
        }

        return returnValue;
    }

    printf("Check point 5\n");


    /*
		Check for possible straight
	*/

	if (straightValue > 0)
    {
		return 500000000000 + (straightValue * 1e9);
	}

    /*
		Check for three of a kind
	*/

    if (threeOfAKind > 0)
    {
        vector<int> values;
        int totalValues = 0;

        for (int index = 14; index > 0; index--)
        {
			if (((stateStraight&(1<<index)) != 0) && (threeOfAKind != index))
            {
				values.push_back(index);
				totalValues ++;
				if (totalValues == 2)
                {
					break;
				}
			}
		}

        long multiplier = 1e7;
        long returnValue = 400000000000 + (threeOfAKind * 1e9);

        for (int i=0; i<2; i++)
        {
            returnValue += (values.at(i) * multiplier);
            multiplier /= 100;
        }

        return returnValue;
    }

    /*
        Two paris
    */

    printf("Check point 6\n");


   if (secondPair > 0)
   {
        int bestKicker = 0;
		for (int index = 14; index > 0; index--)
        {
			if (((stateStraight&(1<<index)) != 0) && (bestPair != index) && (secondPair != index))
            {
				bestKicker = index;
				break;
			}
		}

		return 300000000000 + (bestPair * 1e9) + (secondPair * 1e7) + (bestKicker * 1e5);
   }

    /*
        One pair, good enough imo
    */

  if (bestPair > 0)
  {
    vector<int> values;
    int totalValues = 0;
    for (int index = 14; index > 0; index--)
    {
        if (((stateStraight&(1<<index)) != 0) && (bestPair != index))
        {
            values.push_back(index);
            totalValues ++;
            if (totalValues == 3)
            {
                break;
            }
        }
    }

    long multiplier = 1e7;
    long returnValue = 200000000000 + (bestPair * 1e9);
    for (int i = 0; i < 3; i++)
    {
        returnValue += (values[i] * multiplier);
        multiplier /= 100;
    }

    return returnValue;
  }

    /*
		High card
		Good enough to call though !!!
	*/

    printf("Check point 7\n");


	vector<int> values;
	int totalValues = 0;
	for (int index = 14; index > 0; index--)
    {
		if ((stateStraight&(1<<index)) != 0)
        {
			values.push_back(index);
			totalValues ++;
			if (totalValues == 5)
            {
				break;
			}
		}
	}

    printf("Check point 8\n");


	long multiplier = 1e9;
	long returnValue = 100000000000 + (bestPair * 1e9);
	for (int i = 0; i < 5; i++)
    {
		returnValue += (values[i] * multiplier);
		multiplier /= 100;
	}

    printf("Exiting solve function\n");

	return returnValue;
}