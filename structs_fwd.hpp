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
        //* Class forward declarations
        template <typename T, class Container>
        class thread_queue;
        template <typename T, class Container>
        class thread_stack;
        template <typename T, class Container>
        class thread_list;
        template <typename T, class Container>
        class thread_forward_list;

        //* non-member swap functions
        template <typename T, class C>
        void swap (thread_queue<T, C>& lhs, thread_queue<T, C>& rhs);
        template <typename T, class C>
        void swap (thread_stack<T, C>& lhs, thread_stack<T, C>& rhs);
        template <typename T, class C>
        void swap (thread_list<T, C>& lhs,  thread_list<T, C>& rhs);
        template <typename T, class C>
        void swap (thread_forward_list<T, C>& l, thread_forward_list<T, C>& rh);
    }
}

#endif /* structs_fwd_hpp */
