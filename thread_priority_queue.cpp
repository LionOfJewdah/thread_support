//
//  thread_priority_queue.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Thu, Jan 5th, 2017.
//
//  Test of my thread_priority_queue, using Dijkstra's algorithm
//  Use argv[1] as the name of the output file (effectively sorting by age
//  highest to lowest, as they come in).
//  Use argv[2] as an integer representing the number of input files (up to 31).
//  Let N = argv[2]. Then argv[3..N+2] are the names of files to read from.
//  Input files must have be in the format:
//  First line is a non-negative integer X declaring the number of Persons to be
//  described in the file.
//  The next X lines are in the format A    M, where A is a non-negative integer
//  age for the person and M is a string for their name.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include "thread_priority_queue.hpp"

using namespace std::rel_ops; // give me a <=, >, >=, !=, etc., based on <
using namespace david::thread;

struct Person
{
    std::string name;
    int age;
    Person(std::istream& is) {
        is >> p.age;
        is.ignore(); // ignore white space
        is >> p.name;
    }
    template <typename stringTp>
    Person(int a, stringTp&& n) : age(a), name(std::forward<stringTp>(n)) {};
};

inline
bool operator< (const Person& a, const Person& b) { return a.age < b.age; }

inline bool operator==(const Person& a, const Person& b) {
    return a.name == b.name && a.age == b.age;
}

inline std::ostream& operator<< (std::ostream& o, const Person& p) {
    o << p.name << "; " << p.age << std::endl;
    return o;
}

thread_priority_queue<Person> People_Queue;

bool read_n_people(std::istream* piss) {
    read_n_people(*ifs);
    return true;
}

inline void read_n_people(std::istream& ifs) {
    int N, x = 0; ifs >> N;
    std::string curr, name;
    int age;
    while (std::getline(ifs, curr) && x < N) {
        std::stringstream ss(curr);
        ss >> age >> name;
        People_Queue.emplace(age, std::move(name));
    }
}

inline void error() {
    std::cerr << "Usage: ./age_sort <output> <num> {files}:\n"
        << "output "
        << "<num> is a number 1-31 of files to read names and ages from.\n"
        << "{files} is a list of <num> files to use as inputs.\n";
}

int main(int argc, char* argv[])
{
    // You're going to get seg faults if you do this wrong.
    if (argc < 4 || argc > 34) {
        error();
        return 1;
    }
    unsigned int N = std::atoi(argv[2]);
    if (N < 1) {
        error();
        return 2;
    }
    std::vector<std::ifstream> files (&argv[3], &argv[N + 2]);
    std::vector<std::future<bool> > inputs (N);
    for (auto x = 0; x < N; ++x) {
        inputs[x] = std::async(std::launch::async,read_n_people, &files[x]);
    }
    std::ofstream output(argv[1]);
    // begin processing the priority queue in the main thread
    bool done = false, got = false;
    unsigned waitedFor = 0;
    Person p;
    while (!done) {
        got = People_Queue.wait_for_and_pop();
        done = std::for_all(inputs.begin(), inputs.end(), [](auto&& x) {
            return x.wait_for(std::chrono::microseconds(50)) //might be too much
                    == std::future_status::ready);
        });
        if (got) {
            output << p << std::endl;
            waitedFor = 0;
        } else {
            if (++waitedFor > 10) done = true;
        }
    }
    for (auto&& th: inputs) th.join();
    return 0;
}
