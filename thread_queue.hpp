//
//  thread_queue.hpp
//  thread_support
//
//  Created by David Paul Silverstone on Wed, Jan 4th, 2017.
//
//*  A thread-safe queue implementation, based on that in
//*  Anthony Williams's "C++ Concurrency in Action"

#ifndef thread_queue_hpp
#define thread_queue_hpp

#include "structs_fwd.hpp"
#include <deque>
#include <mutex>
#include <condition_variable>
// #include <memory> // std::shared_ptr

namespace david {
    namespace thread {
        template <typename T, class Container = std::deque<T> >
        class thread_queue {
        public:
            using container_type =  Container;
            using value_type =      T;
            using reference =       value_type&;
            using const_reference = const value_type&
            using size_type =       std::size_t;
        private:
            container_type mData;
            std::mutex mMut;
            std::condition_variable mCV;
        public:
            //* Default constructor. Constructs empty queue.
            thread_queue();
            //* Copy ctor. Performs deep copy, with lock
            thread_queue(const thread_queue& tq);
            //* Copy assignment is deleted.
            thread_queue& operator=(const thread_queue& tq) = delete;
            //* No move assignment or construction.
            ~thread_queue();
            //* Push by value,
            void push(T val);
        protected:

        };
    }
}

#endif /* thread_queue_hpp */
