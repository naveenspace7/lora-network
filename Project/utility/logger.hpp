#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class logger
{
private:
    streambuf *coutbuf;
    ofstream out;
public:
    logger(string out_name = "");
    void exit_logger();
};