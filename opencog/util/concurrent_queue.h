/*
 * opencog/util/concurrent_queue.h
 *
 * Based off of http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
 * Original version by Anthony Williams
 * Modifications by Michael Anderson
 * Modified by Linas Vepstas
 * Updated API to more closely resemble the proposed
 * ISO/IEC JTC1 SC22 WG21 N3533 C++ Concurrent Queues
 * ISO/IEC JTC1 SC22 WG21 P0260R3 C++ Concurrent Queues
 *
 * This differs from P0260R3 in that:
 * 1) The queue here is unbounded in size
 * 2) It doesn't have iterators
 * 3) There is no try_put()
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_CONCURRENT_QUEUE_H
#define _OPENCOG_CONCURRENT_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace opencog
{
/** \addtogroup grp_cogutil
 *  @{
 */

//! Implements a thread-safe queue
template<typename T>
class concurrent_queue
{
    private:
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable condition;
        bool is_canceled;

    public:
        concurrent_queue() : is_canceled(false) {}

        void push(const T& item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!is_canceled) {
                queue.push(item);
                condition.notify_one();
            }
        }

        T pop()
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this] { return !queue.empty() || is_canceled; });
            
            if (is_canceled) {
                return T();
            }

            T item = queue.front();
            queue.pop();
            return item;
        }

        bool try_pop(T& item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty() || is_canceled)
                return false;
            item = queue.front();
            queue.pop();
            return true;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

        void cancel()
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_canceled = true;
            condition.notify_all();
        }

        bool is_closed() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return is_canceled;
        }
};

/** @}*/
} // namespace opencog

#endif // _OPENCOG_CONCURRENT_QUEUE_H
