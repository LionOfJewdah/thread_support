//
//  make_people.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  File to randomly generate people's names and ages.
//  To test my thread_priority_queue with an online sorter
//  argv[1] is the number of output files to generate

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

// declare vectors of first and last names. Take up space
#include "resources/names_f.txt"
#include "resources/names_l.txt"

constexpr auto NFN = FirstNames.size() - 1, NLN = LastNames.size() - 1;

using rand_short = std::uniform_int_distribution<unsigned short>;
rand_short Randy_Marsh(13, 90), Rick_Sanchez(0, NFN), Morty_Smith(0, NLN);
std::default_random_engine UNRG;
auto Randy = [&](){ return Randy_Marsh (UNRG);};
auto Rick  = [&](){ return Rick_Sanchez(UNRG);};
auto Morty = [&](){ return Morty_Smith (UNRG);};

const std::string& getFirst() { return FirstNames[Rick()]; }
const std::string& getLast () { return LastNames [Morty()];}

void writePerson(std::ostream& o = std::cout) {
    o << Randy() << '\t' << getFirst() << ' ' << getLast() << std::endl;
}

inline void argError() {
    std::cerr << "Usage: make_people <N> {files}\n"
        << "<N> is a positive number of files to generate people in.\n";
        << "{files} is either empty or a list of N output file names.\n";
        << "If it is empty, files will be output/people{1..N}.txt\n";
}

std::string fileName() {
    static unsigned XXX = 1;
    return std::string("output/people" + std::string::to_string(XXX) + ".txt");
}

std::vector<std::string>& genFNames(unsigned n = 1) {
    static bool made = false;
    static std::vector<std::string> vec{n};
    if (!made) {
        for (auto&& th: vec) th = fileName();
        made = true;
    }
    return vec;
}

std::vector<std::string>& getFNames(unsigned n = 1) {
    static bool made = false;
    static std::vector<std::string> vec{n};
    static auto i = 0;
    if (!made) {
        for (auto&& th: vec) th = argv[++i + 1];
        made = true;
    }
    return vec;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        argError(); return -1;
    }
    unsigned N = std::atoi(argv[1]);
    if (N < 1) {
        argError(); return 2;
    }
    /* I must have expressions of the same data type (vector::string::iterator)
    *  for the ternary operator that follows. So I construct a vector either
    *  from N calls to fileName(), or argv[2..N+1]. */
    std::vector<std::string> outFiles = argc > 2 ? getFNames(N) : genFNames(N);
    std::vector<std::ofstream> outputs {outFiles.begin(), outFiles.end()};
    for (auto&& out : outputs) {

    }
    return 0;
}
