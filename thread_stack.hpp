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
#include <memory> // std::shared_ptr

namespace david {
    namespace thread {
        struct empty_stack : public std::length::error {
            static constexpr char err_msg[] = "empty stack.";
            const char* what() const noexcept { return err_msg; }
        }

        template <typename T, class Container = std::deque<T> >
        class thread_stack {
        public:
            using container_type  = Container;
            using value_type =      T;
            using reference  =      value_type&;
            using const_reference = const value_type&
            using pointer    =      std::shared_ptr<T>
            using size_type  =      std::size_t;
        private:
            container_type mData;
            mutable std::mutex mMut;

        public:
            //* Default constructor. Constructs empty thread_stack.
            thread_stack() {};

            /** Copy constructor. Performs deep copy, locking the other stack.
            *   @param <ts>: thread_stack to be copied from */
            thread_stack(const thread_stack& ts) {
                std::lock_guard<std::mutex> lk(ts.mMut);
                mData = ts.mData;
            }

            //* Copy assignment is deleted.
            thread_stack& operator=(const thread_stack& ts) = delete;

            //* Explicitly defaulted destructor; thank you RAII
            ~thread_stack() = default;

            //* No move assignment or construction.

            /** Push an object to the thread_stack by value.
            *   @param <val>: value to be pushed to the thread_stack */
            void push(value_type val) {
                std::lock_guard<std::mutex> lk(mMut);
                mData.push_back(std::move_if_noexcept(val));
            }

            /** void try_pop() overload, taking @param value by
            *   reference. This value is overwritten with the previous top
            *   value, which is removed from the stack.
            *   @throw david::thread::empty_stack exception, if the current
            *   thread_stack is empty. */
            void pop(value_type& value) {
                std::lock_guard<std::mutex> lk(mMut);
                if (mData.empty()) throw empty_stack();
                value = mData.back();
                mData.pop_back();
            }

            /** try_pop() overload returning std::shared_ptr to previous top
            *   element, which is removed from the stack.
            *   @return: shared_ptr to value popped from stack
            *   @throw david::thread::empty_stack exception, if the current
            *   thread_stack is empty. */
            pointer try_pop() {
                std::lock_guard<std::mutex> lk(mMut);
                if (mData.empty()) throw empty_stack();
                pointer res(std::make_shared<value_type>(
                    std::move_if_noexcept(mData.back())
                ));
                mData.pop_back();
                return res;
            }

            /** @return: whether the current thread_stack is empty */
            bool empty() const noexcept(
                noexcept(declval<container_type>().empty()))
            {
                std::lock_guard<std::mutex> lk(mMut);
                return mData.empty();
            }

            /** @return: number of elements in the thread_stack */
            size_type size() const noexcept(
                noexcept(declval<container_type>().size()))
            {
                std::lock_guard<std::mutex> lk(mMut);
                return mData.size();
            }
        };
    }
}

#endif /* thread_stack_hpp */
