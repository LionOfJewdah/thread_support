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
#include <memory> // std::shared_ptr

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
            std::condition_variable mCondVar;
            //* Convenience typedefs
            using LGuard = std::lock_guard<std::mutex>;
            using ULock = std::unique_lock<std::mutex>;
            // using _share = std::make_shared<value_type>;
            // template <typename U> using _transfer = std::move_if_noexcept<U>;
        public:
            //* Default constructor. Constructs empty queue.
            thread_queue() {};

            /** Copy constructor. Performs deep copy, locking the other queue.
            *   @param <tq>: thread_queue to be copied from */
            thread_queue(const thread_queue& tq) {
                LGuard lk(other.mMut);
                mData = other.mData;
            }

            //* Copy assignment is deleted.
            thread_queue& operator=(const thread_queue& tq) = delete;

            //* Explicitly defaulted destructor; thank you RAII
            ~thread_queue() = default;

            //* No move assignment or construction.

            /** Push an object to the thread_queue by value.
            *   @param <val>: value to be pushed to the thread_queue */
            void push(value_type val) {
                std::lock_guard<std::mutex> lk(mMut);
                mData.push(std::move_if_noexcept(val));
                mCondVar.notify_one();
            }

            /** thread_queue offers both "try_pop" and "wait_and_pop" functions,
            *   based on what you want to do. */

            /** try_pop() overload returning bool, taking @param value by
            *   reference. This value is overwritten with the previous top
            *   value, which is removed from the queue.
            *   @return: whether a value was successfully popped*/
            bool try_pop(value_type& value) {
                LGuard lk(mMut);
                if (mData.empty())
                    return false;
                value = mData.front();
                mData.pop();
                return true;
            }

            /** try_pop() overload returning std::shared_ptr to previous top
            *   element, which is removed from the queue.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            pointer try_pop() {
                LGuard lk(mMut);
                if (mData.empty())
                    return pointer {};
                pointer res(std::make_shared<value_type>(
                    std::move_if_noexcept(mData.front())
                ));
                mData.pop();
                return res;
            }

            /** void wait_and_pop() overload, taking parameter by reference.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @param value is overwritten with the previous top
            *   value, which is removed from the queue. */
            void wait_and_pop(T& value) {
                std::unique_lock<std::mutex> lk (mMut);
                mCondVar.wait(lk, [this]{return !mData.empty();});
                value = std::move_if_noexcept(mData.front());
                mData.pop();
            }

            /** wait_and_pop() overload returning std::shared_ptr to previous
            *   top element, which is removed from the queue.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            pointer wait_and_pop() {
                std::unique_lock<std::mutex> lk (mMut);
                mCondVar.wait(lk, [this]{return !mData.empty();});
                pointer res(std::make_shared<value_type>(
                    std::move_if_noexcept(mData.front())
                ));
                mData.pop();
                return res;
            }

            /** @return: whether the current thread_queue is empty */
            bool empty() const noexcept(
                noexcept(declval<container_type>().empty()))
            {
                std::lock_guard<std::mutex> lk(mMut);
                return mData.empty();
            }
        };
    }
}

#endif /* thread_queue_hpp */
