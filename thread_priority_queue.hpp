//
//  thread_priority_queue.hpp
//  thread_support
//
//  Created by David Paul Silverstone on Thu, Jan 5th, 2017.
//
//*  A thread-safe priority queue implementation, based on the principles of
//*  Anthony Williams's "C++ Concurrency in Action".
//*  Implemented for Dijkstra's Algorithm

#ifndef thread_priority_queue_hpp
#define thread_priority_queue_hpp

#include "structs_fwd.hpp"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory> // std::shared_ptr

namespace david {
    namespace thread {
        template <typename T, class Container = std::vector<T>,
            typename Compare = std::less<typename Container::value_type> >
        class thread_priority_queue {
        public:
            using value_type       = Container::value_type;
            using reference        = Container::reference;
            using const_reference  = Container::const_reference;
            using size_type        = Container::size_type;
            using container_type   = Container;

        protected:
            Container mData;
            Compare   comp;
            mutable std::mutex mMut;
            std::condition_variable mCondVar;
            //* Convenience typedefs
            using LGuard = std::lock_guard<std::mutex>;
            using ULock = std::unique_lock<std::mutex>;

        private:
            using std::make_heap;
            using std::push_heap;
            using std::pop_heap;
            /** Private pop member function. Requires mutex to be locked.
            *   Removes the top element of the priority queue, and internally
            *   sorts according to comp.
            *   Called by all the public thread-safe pop function variants. */
            void pop() {
                std::pop_heap(mData.begin(), mData.end(), comp);
                mData.pop_back();
            }

        public:
            /**
             *  @brief  Default constructor creates no elements.
             */
            explicit
            thread_priority_queue(const Compare& __x, const Container& __c)
            : mData(__c), comp(__x)
            { std::make_heap(mData.begin(), mData.end(), comp); }

            explicit
            thread_priority_queue(const Compare& __x = _Compare(),
      		     Container&& __c = Container())
            : mData(std::move(__c)), comp(__x)
            { std::make_heap(mData.begin(), mData.end(), comp); }

            ~thread_priority_queue() = default;

            /**
            *  @brief  Builds a %thread_priority_queue from a range.
            *  @param  __first  An input iterator.
            *  @param  __last  An input iterator.
            *  @param  __x  A comparison functor--a strict weak ordering.
            *  @param  __c  An initial sequence with which to start.
            *
            *  Begins by copying @a __c, inserting a copy of the elements
            *  from @a [first,last) into the copy of @a __c, then ordering
            *  the copy according to @a __x.
            */

            template<typename InputIterator>
            thread_priority_queue(InputIterator __first, InputIterator __last,
                 const Compare& __x, const Container& __c)
            : mData(__c), comp(__x)
            {
                mData.insert(mData.end(), __first, __last);
                make_heap(mData.begin(), mData.end(), comp);
            }

            template<typename InputIterator>
            priority_queue(InputIterator __first, InputIterator __last,
                 const Compare& __x = Compare(), Container&& __c = Container())
            : mData(std::move(__c)), comp(__x)
            {
                mData.insert(mData.end(), __first, __last);
                make_heap(mData.begin(), mData.end(), comp);
            }
            /** @return: whether the current thread_queue is empty */
            bool empty() const noexcept(
                noexcept(declval<container_type>().empty()))
            {
                std::lock_guard<std::mutex> lk(mMut);
                return mData.empty();
            }

            /** @return: number of elements in the thread_queue */
            size_type size() const noexcept(
                noexcept(declval<container_type>().size()))
            {
                std::lock_guard<std::mutex> lk(mMut);
                return mData.size();
            }

            /**
            *  @brief  Add data to the %thread_priority_queue.
            *  @param  __x  Data to be added.
            *
            *  This is a typical %queue operation.
            *  The time complexity of the operation depends on the underlying
            *  sequence.
            */
            void push(const value_type& __x) {
                std::lock_guard<std::mutex> lk(mMut);
                mData.push_back(__x);
                push_heap(mData.begin(), mData.end(), comp);
                mCondVar.notify_one();
            }

            void push(value_type&& __x) {
                LGuard lk(mMut);
                mData.push_back(std::move(__x));
                push_heap(mData.begin(), mData.end(), comp);
                mCondVar.notify_one();
            }

            /** Emplacement function.
            *   @param <args...>: parameter pack to forward to underlying
            *   container for back-emplacement */
            template<typename... Args>
            void emplace(Args&&... args) {
                LGuard lk(mMut);
                mData.emplace_back(std::forward<Args>(args)...);
                push_heap(mData.begin(), mData.end(), comp);
                mCondVar.notify_one();
            }

            /** thread_priority_queue offers "try_pop" and "wait_and_pop"
            *   functions, as well as "wait_for_and_pop",
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
                pop();
                return true;
            }

            /** try_pop() overload returning std::shared_ptr to previous top
            *   element, which is removed from the queue.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            pointer try_pop() {
                LGuard lk(mMut);
                if (mData.empty())
                    return pointer();
                pointer res(std::make_shared<value_type>(
                    std::move_if_noexcept(mData.front())
                ));
                pop();
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
                pop();
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
                pop();
                return res;
            }

            /** bool wait_for_and_pop() overload, taking parameter by reference.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @param value is overwritten with the previous top
            *   value, which is removed from the queue.
            *   @param d: duration to wait for.
            *   @return whether a thing was popped.*/
            template <typename Rep, class Period = std::ratio<1> >
            bool wait_for_and_pop(T& value,
                const std::chrono::duration<Rep, Period>& d)
            {
                std::unique_lock<std::mutex> lk (mMut);
                bool b=mCondVar.wait_for(lk, d, [this]{return !mData.empty();});
                if (!b) return false;
                value = std::move_if_noexcept(mData.front());
                pop();
                return true;
            }

            /** wait_for_and_pop() overload, returning std::shared_ptr to
            *   previous top element, which is removed from the queue.
            *   Waits until the thread can acquire a lock on the mutex, then
            *   pops from the queue.
            *   @param d: duration to wait for.
            *   @return: shared_ptr to value popped from queue, or the default
            *   std::shared_ptr (nullptr) if the pop was unsuccessful */
            template <typename Rep, class Period = std::ratio<1> >
            pointer wait_for_and_pop(const std::chrono::duration<Rep, Period>&d)
            {
                std::unique_lock<std::mutex> lk (mMut);
                bool b=mCondVar.wait_for(lk, d, [this]{return !mData.empty();});
                if (!b) return pointer();
                pointer res(std::make_shared<value_type>(
                    std::move_if_noexcept(mData.front())
                ));
                pop();
                return res;
            }

            /** A transactional swap member function.
            * @param <rhs>: thread_priority_queue to swap with *this */
            void swap(thread_priority_queue& rhs) {
                if (this == &rhs) return;
                std::lock(mMut, rhs.mMut);
                using std::swap;
                LGuard lock_a(mMut,     std::adopt_lock);
                LGuard lock_b(rhs.mMut, std::adopt_lock);
                swap(mData, rhs.mData);
                swap(comp,  rhs.comp);
            }

        };

        template <typename T, class C, class Cm>
        inline void swap (thread_priority_queue<T, C, Cm>& lhs,
            thread_priority_queue<T, C, Cm>& rhs) {
            lhs.swap(rhs);
        }
    }
}

#endif /* thread_priority_queue_hpp */
