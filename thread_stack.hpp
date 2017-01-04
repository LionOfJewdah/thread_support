//
//  thread_stack.hpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  A thread-safe stack implementation, based on that in
//  Anthony Williams's "C++ Concurrency in Action"

#ifndef thread_stack_hpp
#define thread_stack_hpp

#include "structs_fwd.hpp"
#include <deque>

namespace david {
    namespace thread {
        template <typename T, class Container = std::deque<T> >
        class thread_stack {
        private:
        public:
            thread_stack();
            thread_stack(const thread_stack& ts);
            thread_stack& operator=(const thread_stack& ts) = delete;
            ~thread_stack();
        protected:

        };
    }
}

#endif /* thread_stack_hpp */
