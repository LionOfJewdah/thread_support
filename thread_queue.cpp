//
//  thread_queue.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  Test of my thread_queue

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "thread_queue.hpp"
#include <thread>
#include <future>
#include <chrono>

using namespace david::thread;

constexpr unsigned ROW_BOAT_WORDS = 18;
constexpr unsigned BLA_BLA_BLAH_WORDS = 332;

void processIfstream(std::ifstream& is, std::vector<std::string>& vs) {
    std::string word, line;
    while( getline(is, line) ) {
        std::stringstream buffer(line);
        std::cout << line << std::endl << std::flush;
        while (buffer >> word) vs.push_back(word);
    }
    std::endl(std::cout);
};

void singOutOfTune (const std::vector<std::string>& lyrics,
    thread_queue<std::string>& tq)
{
    static std::atomic<std::size_t> where = 0;
    thread_local std::vector<std::string> thread_lyrics (lyrics.size());
    std::size_t j = 0;
    for ( ; where.load() < lyrics.size(); ) {
        thread_lyrics[j++] = lyrics[where++.load()];
    }
    for (auto it = thread_lyrics.begin(), jt = it + j; it != jt; ++it) {

    }
}

int main()
{
    std::ifstream input1 ("resources/row_your_boat.txt",ios::in);
    std::ifstream input2 ("resources/kesha.txt",ios::in);
    if (!(input1.is_open() && input2.is_open())) {
        std::cout << "Unable to open input files." << std::endl;
        return 1;
    }
    std::vector<std::string> boatVec, keshaVec;
    boatVec.reserve(ROW_BOAT_WORDS); keshaVec.reserve(BLA_BLA_BLAH_WORDS);
    auto fut1 = std::async (std::launch::async, processIfstream, input1,
        boatVec);

    std::chrono::milliseconds span (100);
    // do something while waiting for function to set future:
    std::cout << "Processing file resources/row_your_boat.txt:" << std::endl;
    while (fut1.wait_for(span)==std::future_status::timeout)
      std::cout << '.' << std::flush;
    fut1.get();

    std::cout << "Processing file resources/kesha.txt:" << std::endl;
    auto fut2 = std::async (std::launch::async, processIfstream, input2,
        keshaVec);
    while (fut2.wait_for(span)==std::future_status::timeout)
        std::cout << '.' << std::flush;
    fut2.get();

    // I know I could do these concurrently, but it's funnier this way

    thread_queue<std::string> tqString;

    return 0;
}
