//
//  structs_fwd.hpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  Forward declaration header for thread-safe data structures,
//  based on those in Anthony Williams's "C++ Concurrency in Action"

#ifndef structs_fwd_hpp
#define structs_fwd_hpp

#include <utility> // std::move, std::move_if_noexcept

namespace david {
    namespace thread {
        template <typename T, class Container>
        class thread_queue;
        template <typename T, class Container>
        class thread_stack;
        template <typename T, class Container>
        class thread_list;
        template <typename T, class Container>
        class thread_forward_list;
    }
}

#endif /* structs_fwd_hpp */
