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

const auto NFN = FirstNames.size() - 1, NLN = LastNames.size() - 1;

using rand_short = std::uniform_int_distribution<unsigned short>;
// using rand_byte  = std::uniform_int_distribution<unsigned char>;
std::default_random_engine UNRG;
rand_short Randy_Marsh(8, 90), Rick_Sanchez(0, NFN), Morty_Smith(0, NLN);
rand_short Bojack_Horseman(128, 20000), Mike_Shindler(0, 11), Aaron_Cote(0, 30);
auto Randy  = [&](){ return Randy_Marsh   (UNRG); }; // for ages
auto Rick   = [&](){ return Rick_Sanchez  (UNRG); }; // for first names
auto Morty  = [&](){ return Morty_Smith   (UNRG); }; // for last names
auto Bojack = [&](){ return Bojack_Horseman(UNRG);}; // for #people per file
auto Shindy = [&](){ return Mike_Shindler (UNRG); }; // for age months
auto Cote   = [&](){ return Aaron_Cote    (UNRG); }; // for age days

const std::string& getFirst() { return FirstNames[Rick()]; }
const std::string& getLast () { return LastNames [Morty()];}

void writePerson(std::ostream& o = std::cout) {
    o << Randy() << ' ' << Shindy() << ' ' << Cote()
        << '\t' << getFirst() << ' ' << getLast() << std::endl;
}

inline void argError() {
    std::cerr << "Usage: make_people <N> {files}\n"
        << "<N> is a positive number of files to generate people in.\n"
        << "{files} is either empty or a list of N output file names.\n"
        << "If it is empty, files will be output/people{1..N}.txt\n";
}

std::string fileName() {
    static unsigned _xxx = 1;
    return std::string("output/people" + std::to_string(_xxx++) + ".txt");
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

std::vector<std::string>& getFNames(char** argv, unsigned n = 1) {
    static bool made = false;
    static std::vector<std::string> vec{n};
    static auto i = 0;
    if (!made) {
        for (auto&& th: vec) th = argv[++i + 1];
        made = true;
    }
    return vec;
}

template <typename T>
inline std::size_t _s_int(T* pT) { return reinterpret_cast<std::size_t>(pT);}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        argError(); return -1;
    }
    unsigned N = std::atoi(argv[1]);
    if (N < 1) {
        argError(); return 2;
    }
    if (argc > 2) { // make random seeds from file names of input arguments
        std::seed_seq seedy {_s_int(&argv[1]), _s_int(&argv[N])};
        UNRG.seed(seedy);
    }
    /* I must have expressions of the same data type (vector::string::iterator)
    *  for the ternary operator that follows. So I construct a vector either
    *  from N calls to fileName(), or argv[2..N+1]. */
    auto&& a = argv;
    std::vector<std::string> outFiles= argc > 2 ? getFNames(a,N) : genFNames(N);
    std::vector<std::ofstream> outputs {outFiles.begin(), outFiles.end()};
    for (auto&& out : outputs) {
        auto X = Bojack();
        out << X << std::endl;
        for (auto k = 0; k < X; ++k) {
            writePerson(out);
        }
    }
    return 0;
}
