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
#include <thread>
#include <future>
#include <chrono>
#include "thread_queue.hpp"

using namespace david::thread;

constexpr unsigned ROW_BOAT_WORDS = 18;
constexpr unsigned BLA_BLA_BLAH_WORDS = 332;

void processIfstream(std::ifstream* piss, std::deque<std::string>* pds) {
    auto&& is = *piss; auto&& ds = *pds;
    std::string word, line;
    while( std::getline(is, line) ) {
        std::stringstream buffer(line);
        //std::cout << line << std::endl << std::flush;
        while (buffer >> word) ds.push_back(word);
    }
    std::endl(std::cout);
};

std::atomic<bool> Singing;
constexpr std::chrono::milliseconds span (100);

void singOutOfTune (thread_queue<std::string>* ptq) {
    auto&& tq = *ptq;
    std::vector<std::string> thread_lyrics (tq.size());
    std::size_t i = 0;
    for (; !tq.empty(); ++i) {
        // tq.wait_and_pop(thread_lyrics[i]);
        if (!tq.wait_for_and_pop(thread_lyrics[i], span)) {
            bool b = false;
            for (auto attempt = 0; attempt <= 10 && !b; ++attempt) {
                b = tq.wait_for_and_pop(thread_lyrics[i], span);
                if (attempt == 10) std::this_thread::yield();
            }
        }
    }
    unsigned j = 0;
    std::cout << "\nThread " << std::this_thread::get_id()<< std::flush <<":\n";
    for (auto it = thread_lyrics.begin(), jt = it + i; it != jt; ++it, ++j) {
        std::cout << *it << ' ';
        if (j % 5 == 4) std::cout << std::endl; // << std::flush;
        if (j % 20 == 19) std::cout << std::endl << std::flush;
    }
}

int main()
{
    std::ifstream input1 ("resources/hamlet.txt");
    std::ifstream input2 ("resources/kesha.txt");
    if (!(input1.is_open() && input2.is_open())) {
        std::cout << "Unable to open input files." << std::endl;
        return 1;
    }
    unsigned hwc_ = std::thread::hardware_concurrency();
    unsigned num_threads = (hwc_ ? hwc_ - 1: 4);
    /** ^ is what you should do to use the maximum number of processor cores
    *   (the number available, minus 1 for main()).
    *   Alternatively, below is one more for craps, giggles and serialization.
    *   What follows would for the sake of laughs. If you want something
    *   "reasonable" for an algorithm, keep the lines commented as is. */
    // unsigned num_threads = 3*(hwc_ ? hwc_ - 1: 4);
    std::cout << num_threads << " threads available. (" << hwc_ << ")\n";

    std::deque<std::string> boatDec, keshaDec;
    auto fut1 = std::async (std::launch::async, processIfstream, &input1,
        &boatDec);

    // do something while waiting for function to set future:
    std::cout << "Processing file resources/hamlet.txt" << std::endl;
    while (fut1.wait_for(span)==std::future_status::timeout)
        std::cout << '.' << std::flush;

    fut1.get();

    thread_queue<std::string> tqRowBoat(std::move(boatDec));
    std::cout << "Done processing row_your_boat.txt." << std::endl
        << "Preparing to sing out of key." << std::endl;
    std::vector<std::thread> songThreads (num_threads);
    Singing = true;
    for (auto&& th : songThreads) {
        th = std::thread(singOutOfTune, &tqRowBoat);
    }
    for (auto && th : songThreads) {
        th.join();
    }
    Singing = false;
    std::cout << std::endl << std::endl << "Beautiful." << std::endl << '\n';

    std::cout << "Processing file resources/kesha.txt:" << std::endl;
    auto fut2 = std::async (std::launch::async, processIfstream, &input2,
        &keshaDec);
    while (fut2.wait_for(span)==std::future_status::timeout)
        std::cout << '.' << std::flush;
    fut2.get();
    thread_queue<std::string> tqKesha (std::move(keshaDec));
    std::cout << "Done processing kesha.txt." << std::endl
        << "Preparing to sing drunk with autotune." << std::endl;
    Singing = true;
    for (auto&& th : songThreads) {
        th = std::thread(singOutOfTune, &tqKesha);
    }
    for (auto && th : songThreads) {
        th.join();
    }
    Singing = false;
    std::cout <<"\n\n*************\nZip your lip like a padlock.\n" <<std::endl;

    // I know I could do these concurrently, but it's funnier this way

    return 0;
}
