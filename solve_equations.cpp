//
//  solve_equations.cpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  File to solve math equations, adding, subtracting, multiplying and
//  dividing doubles.
//  To test my thread_queue

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <sstream>
#include <future>
#include <chrono>
#include <random>
#include <map>

struct operation {
    double  a;
    char    op;
    double  b;
    constexpr explicit operation(double x, char o, double y) noexcept
    : a(x), op(o), b(y) {}
    explicit operation(std::stringstream& ss) {
        ss >> a >> op >> b;
    }
    explicit operation(const std::string& s) {
        std::stringstream ss(s);
        ss >> a >> op >> b;
    }

    double operator() const noexcept () {
        switch (op) {
            case '+':
                return a + b;
            case '-':
                return a - b;
            case '*':
                return a * b;
            case: '/':
                return a / b;
            default: return 0.0;
        }
    }

    constexpr double evaluate() const noexcept() {
        return this->operator();
    }

    constexpr operator double() const noexcept() {
        return this->operator();
    }

    operation(const operation& rhs) noexcept = default;
    operation(operation&& rhs) noexcept = default;
    operation& operator= (const operation& rhs) noexcept = default;
    operation& operator= (operation&& rhs) noexcept = default;
    ~operation() = default;
};

inline operator<< (std::ostream& o, const operation& Op) {
    o << Op.a << ' ' << Op.op << ' ' << Op.b;
    return o;
}

std::map<unsigned, operation>& Equations() {
    static std::map<unsigned, operation> eqns;
    return eqns;
}

// std::mutex map_mutex;

void add_equation(std::stringstream& ss) {
    unsigned x;
    ss >> x;
    auto pear = std::make_pair(x, operation(ss));
    // std::lock_guard<std::mutex> lk(map_mutex);
    Equations().insert(pear);
}

void init_map(const std::string& inFile) {
    std::ifstream input (inFile);
    if (!input.is_open()) throw std::domain_error("Input not open.");
    std::string s;
    while (std::getline(input, s)) {
        std::stringstream ss(s);
        add_equation(ss);
    }
}

/* Decided against using these
std::pair<unsigned, double> pairwise_eval(const typename
    decltype(Equations())::value_type& pear)
{
    return std::pair(std::piecewise_construct, pear.first, pear.second());
}

std::map<unsigned, double>& Solutions() {
    static std::map<unsigned, double> solns;
    return solns;
}*/

void print_map(std::ostream& o = std::cout) {
    for (const auto& eq : Equations()) {
        o << eq.first << ": " << eq.second << " = " << eq.second() << '\n';
    }
}


int main(int argc, char* argv[])
{
    std::string inFile = (argc > 1 ? argv[1] : "resources/equations.txt");
    std::string outFile = (argc > 2 ? argv[2] : "output/solutions.txt");
    try init_map(inFile);
    catch (std::domain_error& q) {
        std::cerr << q.what() << std::endl;
        return -1;
    }

    std::ofstream output (outFile);
    if (!output.is_open()) {
        std::cerr << "Could not open output file.\n";
        return -2;
    }

    return 0;
}
