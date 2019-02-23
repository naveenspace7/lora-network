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

void logger::exit_logger()
{
    cout.rdbuf(coutbuf);
}

// reuse this function
vector<int> get_timestamp()
{
  vector<int> temp(3);
  time_t now = time(0);
  struct tm *mytime = localtime(&now);
  temp[0] = mytime->tm_hour;
  temp[1] = mytime->tm_min;
  temp[2] = mytime->tm_sec;
  return temp;
}

logger::logger(string out_name)
{
    if (out_name == "")
    {
        stringstream strstrm;
        vector<int> timestamp = get_timestamp();
        strstrm << timestamp[0] << timestamp[1] << timestamp[2];
        out_name = strstrm.str();
    }

    out_name += ".log";
    cout << "Logging to the file " << out_name << endl;
    out.open(out_name, std::ofstream::out | std::ofstream::app);
    coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());
}

// Test code:
// int main()
// {
//     cout << "entering main func" << endl;
//     // logger mylog("temp_check");
//     logger mylog;
//     cout << "entering into main func expecting into a log file" << endl;

//     cout << "hello world this is a test and it should actually work" << endl;

//     // ofstream out("out.txt");
//     // cout << "initial test for the applications to run around" << endl;
//     // streambuf *coutbuf = cout.rdbuf();
//     // cout.rdbuf(out.rdbuf());
//     // cout.rdbuf(coutbuf);
 
//     mylog.exit_logger();
//     cout << "Ah I'm done for the day." << endl;
// }