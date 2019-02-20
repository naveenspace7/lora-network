#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void f()
{
    std::string line;
    while(std::getline(std::cin, line))  //input from the file in.txt
    {
        std::cout << line << "\n";   //output to the file out.txt
    }
}
int main()
{
    std::ifstream in("in.txt");
    std::streambuf *cinbuf = std::cin.rdbuf(); //save old buf
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!

    std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    std::cout << "what the fuck is happening I don't know" << std::endl;
    std::cout << "it all takes time and patience in reading the proper books" << std::endl;

    // std::string word;
    // std::cin >> word;           //input from the file in.txt
    // std::cout << word << "  ";  //output to the file out.txt

    // f(); //call function


    std::cin.rdbuf(cinbuf);   //reset to standard input again
    std::cout.rdbuf(coutbuf); //reset to standard output again

}

// enum REDIRECT = {TO_FILE, TO_STDOUT};

// void file_redirect(REDIRECT direction)
// {
//     streambuf *
//     if (direction == TO_FILE)
//     {
//         ofstream log_file("log_file.txt");
//         cout.rdbuf(log_file.rdbuf());
//     }

//     else if (direction == TO_STDOUT)
//     {
//         cout.rdbuf(coutbuf)
//     }

// }


class logger
{
public:
    static void enable(string filename = NULL);
    static void disable();
private:


};

void logger::enable(string filename)
{
    if (filename == NULL)
    {
        // get the timestamp and do further processing

    }
    ofstream log_file(filename);
    
}