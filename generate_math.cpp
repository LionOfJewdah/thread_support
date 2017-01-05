//
//  generate_math.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  File to randomly generate math equations
//  To test my thread_queue with an equation solver

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <sstream>
#include <future>
#include <chrono>
#include <random>
#include <atomic>

using uniform_dist  = std::uniform_real_distribution<double>; // from <random>
using rand_int      = std::uniform_int_distribution<unsigned char>;

constexpr char ops [] = { '+', '-', '*', '/'};
unsigned hwc = std::thread::hardware_concurrency();
unsigned num_threads = (hwc ? hwc - 1 : 3);
std::vector<uniform_dist> serializable (num_threads,
    uniform_dist(-50000.0, 50000.0));
rand_int randz(0, 3);

std::default_random_engine UNRG;

// inline typename uniform_dist::result_type makeRand(unsigned i = 0) {
inline double makeRand(unsigned i = 0) {
    return (i < num_threads ? serializable[i](UNRG) : 0.0);
} // this type is double, but I have it more generic

std::atomic<unsigned> so_far_made(0);

void make_thread_equations(unsigned thNum, unsigned num_per_thread,
    std::ostream* po = &std::cout) {
    auto && o = *po;
    for (unsigned i = 0; i < num_per_thread; ++i) {
        std::stringstream ss;
        ss << so_far_made++ << ": " << makeRand(thNum) << ' '
            << ops[randz(UNRG)] << ' ' <<makeRand(thNum) << '\n';
        o << ss.str() << std::flush;
    }
}

inline void reset_so_far () noexcept {
    so_far_made = 0;
}

int main(int argc, char* argv[])
{
    unsigned how_many_eqns = 40000;
    if (argc > 1) how_many_eqns = std::atoi(argv[1]);
    std::string fileName = "resources/equations.txt";
    if (argc > 2) fileName = argv[2];
    std::ofstream output(fileName);
    if (!output.is_open()) {
        std::cerr << "Could not open file " << fileName << std::endl;
        return 1;
    }
    unsigned char x = 0;
    std::vector<std::future<void>> vfut (num_threads);
    auto num_per_thread = how_many_eqns/(num_threads + 1);
    for (auto&& thing : vfut) {
        thing = std::async(std::launch::async, make_thread_equations, x++,
            num_per_thread, &output);
    }
    make_thread_equations(x, how_many_eqns - num_threads * num_per_thread,
        &output);
    for (auto&& thing : vfut) thing.get();
    std::cout << "Done making equations." << std::endl;
    return 0;
}
