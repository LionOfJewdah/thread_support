//
//  thread_priority_queue.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Thu, Jan 5th, 2017.
//
//  Test of my thread_priority_queue, using Dijkstra's algorithm
//  Use argv[2] as the name of the output file (effectively sorting by age
//  highest to lowest, as they come in).
//  Use argv[1] as an integer representing the number of input files (up to 31).
//  Let N = argv[1]. Then argv[3..N+2] are the names of files to read from.
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
#include <algorithm>
#include "thread_priority_queue.hpp"

using namespace std::rel_ops; // give me a <=, >, >=, !=, etc., based on <
using namespace david::thread;

typedef unsigned short ushort;
typedef unsigned char  uchar;  // for laziness

struct Person
{
    struct Age {
        unsigned short years;
        unsigned char  months, days;
        Age(ushort y, uchar m = 0, uchar d = 0): years(y), months(m), days(d) {}
    } age;
    std::string name;
    Person() : age(0) {};
    template <typename stringTp>
    Person(ushort y, uchar m, uchar d, stringTp&& n)
    : age(y, m, d), name(std::forward<stringTp>(n)) {};
};

inline bool operator==(const struct Person::Age&a, const struct Person::Age&b) {
    return a.years == b.years && a.months == b.months && a.days == b.days;
};

inline bool operator<(const struct Person::Age& a, const struct Person::Age& b){
    return a.years < b.years || (a.years == b.years && a.months < b.months)
        ||  (a.years == b.years && a.months == b.months && a.days < b.days);
};

inline
bool operator< (const Person& a, const Person& b) { return a.age < b.age; };

inline bool operator==(const Person& a, const Person& b) {
    return a.name == b.name && a.age == b.age;
};

inline std::ostream& operator<< (std::ostream& o, const struct Person::Age& a) {
    o << a.years << ' ' << ushort(a.months) << ' ' << ushort(a.days);
    return o;
};

inline std::ostream& operator<< (std::ostream& o, const Person& p) {
    o << p.name << ";\t" << p.age;
    return o;
}

thread_priority_queue<Person> People_Queue;

unsigned countIn = 0;

inline void read_n_people(std::istream& ifs) {
    int N, x = 0;
    std::cout << "0x" << std::hex << std::this_thread::get_id() << ": "
        << std::dec << std::flush;
    ifs >> N; countIn += N;
    std::cout << N << '\n';
    std::string curr, name, name2;
    unsigned short age, mo, dy;
    while (std::getline(ifs, curr) && x++ <= N) {
        std::stringstream ss(curr);
        ss >> age >> mo >> dy >> name >> name2;
        People_Queue.emplace(age, mo, dy, std::move(name + " " + name2));
    }
}

bool pread_n_people(std::istream* piss) {
    if (!piss /*|| piss->fail()*/) return false;
    read_n_people(*piss);
    return true;
}

inline void error() {
    std::cerr << "Usage: ./age_sort <num> <output> {files}:\n"
        << "<num> is a number 1-31 of files to read names and ages from.\n"
        << "<output> is a file to write results to.\n"
        << "{files} is a list of <num> files to use as inputs.\n";
}

int main(int argc, char* argv[])
{
    // You're going to get seg faults if you do this wrong.
    if (argc < 4 || argc > 34) {
        error();
        return 1;
    }
    unsigned int N = std::atoi(argv[1]);
    if (N < 1) {
        error();
        return 2;
    }

    std::vector<std::ifstream> files (&argv[3], &argv[N + 3]); //[first, last)
    std::vector<std::thread> inputs (N);
    for (unsigned x = 0; x < N; ++x) {
        inputs[x] = std::thread(pread_n_people, &files[x]);
    }
    std::ofstream output(argv[2]);
    // begin processing the priority queue in the main thread
    bool done = false, got = false;
    unsigned waitedFor = 0;
    Person p;
    while (!done) {
        got = People_Queue.wait_for_and_pop(p, std::chrono::milliseconds(10));
        done = std::none_of(inputs.begin(), inputs.end(), [](auto&& x) {
            return x.joinable();
        });
        if (got) {
            output << p << std::endl;
            waitedFor = 0;
        } else {
            if (++waitedFor > 10) {
                done = true;
                std::cout << "Exited due to timeout.\n";
            }
        }
    }
    // std::vector<bool> success (N); unsigned i = 0;
    for (auto&& th: inputs) th.join();
    std::cout << "Received " << countIn << " objects. " << std::endl;
    std::cout << "Exiting." << std::endl;
    return 0;
}
