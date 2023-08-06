#include "Utils.hpp"

vector<double> FilledArrayDouble(int lenght, double values)
{
    vector<double> v;
    for (int i=0; i<lenght; i++)
    {
        v.push_back(values);
    }

    return v;
}