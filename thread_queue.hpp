//
//  thread_queue.hpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//  A thread-safe queue implementation, based on that in
//  Anthony Williams's "C++ Concurrency in Action"

#ifndef thread_queue_hpp
#define thread_queue_hpp

#include "structs_fwd.hpp"
#include <deque>

namespace david {
    namespace thread {
        template <typename T, class Container = std::deque<T> >
        class thread_queue {
        private:
        public:
            thread_queue();
            thread_queue(const thread_queue& tq);
            thread_queue& operator=(const thread_queue& tq) = delete;
            ~thread_queue();
        protected:

        };
    }
}

#endif /* thread_queue_hpp */
