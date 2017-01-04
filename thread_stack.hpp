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
#include <mutex>
#include <stdexcept>
//#include <memory> // std::shared_ptr

namespace david {
    namespace thread {
        struct empty_stack : public std::length::error {
            static constexpr char err_msg[] = "empty stack.";
            const char* what() const noexcept { return err_msg; }
        }

        template <typename T, class Container = std::deque<T> >
        class thread_stack {
        public:
            using container_type =  Container;
            using value_type =      T;
            using reference =       value_type&;
            using const_reference = const value_type&
            using size_type =       std::size_t;
        private:
            std::mutex mMut;
            container_type mData;
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
