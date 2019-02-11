#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

class logger
{
public:
    logger(string filename);
    logger();
    void log(string);
    ~logger();

private:
    ofstream handle;
    void init(string);
};

void logger::init(string filename)
{
    filename += ".log";
    handle.open(filename, ofstream::out | ofstream::trunc);
    
    if(handle.is_open()) cout << "Logging to " << filename << '\n';
}