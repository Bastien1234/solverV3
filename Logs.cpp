#include <string>
#include <fstream>
#include <iostream>

void text_log(bool enabled, std::string function_name)
{
    if (enabled == false) { return ; }

    std::ofstream myFile;
    myFile.open("logs.txt");

    myFile << function_name << std::endl;
}