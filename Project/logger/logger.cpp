#include "logger.hpp"

logger::logger(string filename) {
    init(filename);
}

logger::logger() {
    time_t now = time(0);
    struct tm *mytime = localtime(&now);
    string timestamp = "log_file_";
    timestamp += to_string(mytime->tm_hour) + to_string(mytime->tm_min) + to_string(mytime->tm_sec);
    init(timestamp);
}

void logger::log(string statement) {
    if(handle.is_open()) handle << statement << endl;
    else cout << "Failed to log into the log file" << endl;
}

logger::~logger() {
    handle.close();
}

// int main()
// {
//     logger l_file;
//     l_file.log("whatever I am seeing is not true at all");
// }