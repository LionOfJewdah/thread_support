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

            

        };
    }
}

#endif /* thread_priority_queue_hpp */
