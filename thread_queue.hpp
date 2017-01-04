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
        //* public member type aliases
        public:
            using container_type  = Container;
            using value_type =      T;
            using reference  =      value_type&;
            using const_reference = const value_type&
            using pointer    =      std::shared_ptr<T>
            using size_type  =      std::size_t;
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
            void push(value_type val);

            //* thread_queue offers both "try_pop" and "wait_and_pop" functions,
            //* based on what you want to do.

            /** try_pop() overload returning bool, taking @param value by
            *   reference. This value is overwritten with the previous top
            *   value, which is removed from the queue.
            *   @return: whether a value was successfully popped*/
            bool try_pop(value_type& value);

            /** try_pop() overload returning std::shared_ptr to previous top
            *   element, which is removed from the queue.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            pointer try_pop();

            /** void wait_and_pop() overload, taking parameter by reference.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @param value is overwritten with the previous top
            *   value, which is removed from the queue. */
            void wait_and_pop(T& value);

            /** wait_and_pop() overload returning std::shared_ptr to previous
            *   top element, which is removed from the queue.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            pointer wait_and_pop();

        };
    }
}

#endif /* thread_queue_hpp */
