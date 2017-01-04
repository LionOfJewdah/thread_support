#include <cstdio>
#include <thread>
#include <future>
#include <vector>
#include <type_traits> // std::result_of

void show(char c) {
    printf("%c", c);
}

template< class T >
using result_of_t = typename std::result_of<T>::type;

namespace david {
    template <typename T, std::size_t N>
    inline constexpr std::size_t array_size(const T (&)[N]) { return N; }
}

constexpr char message[] = "Hello World.";

auto twelve = david::array_size(message) - 1; // null-terminated message

int main(int argc, char* argv[])
{
    std::vector<decltype(std::async(show, std::declval<char>()))> vfut;
    vfut.reserve (twelve);
    for (auto i = 0; i < twelve; ++i) {
        vfut[i] = std::async(std::launch::async, show, message[i]);
    }
    for (auto&& thing : vfut) {
        thing.get();
    }
    printf("\n");
    return 0;
}
