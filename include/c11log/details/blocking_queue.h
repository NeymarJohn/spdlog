#pragma once

// blocking_queue:
// A blocking multi-consumer/multi-producer thread safe queue.
// Has max capacity and supports timeout on push or pop operations.

#include <cstddef>
#include <chrono>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace c11log {
namespace details
{
template<typename T>
class blocking_queue {
public:
    using queue_t = std::queue<T>;
    using size_type = typename queue_t::size_type;
    using clock = std::chrono::system_clock;

    explicit blocking_queue(size_type max_size) :_max_size(max_size), _q()
    {}
    blocking_queue(const blocking_queue&) = delete;
    blocking_queue& operator=(const blocking_queue&) = delete;
    blocking_queue& operator=(const blocking_queue&) volatile = delete;
    ~blocking_queue() = default;

    size_type size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _q.size();
    }

    // Push copy of item into the back of the queue. 
    // If queue is full, block the calling thread util there is room or timeout have passed.
    // Return: false on timeout, true on successful push.
    template<class Duration_Rep, class Duration_Period>
    bool push(const T& item, const std::chrono::duration<Duration_Rep, Duration_Period>& timeout)
    {
        std::unique_lock<std::mutex> ul(_mutex);
        if (_q.size() >= _max_size)
        {
            if (!_item_popped_cond.wait_until(ul, clock::now() + timeout, [this]() { return this->_q.size() < this->_max_size; }))
                return false;
        }
        _q.push(item);
        if (_q.size() == 1)
        {
            ul.unlock();
            _item_pushed_cond.notify_one();
        }
        return true;
    }

    // Push copy of item into the back of the queue. 
    // If queue is full, block the calling thread until there is room
    void push(const T& item)
    {
        while (!push(item, std::chrono::hours::max()));
    }

    // Pop a copy of the front item in the queue into the given item ref
    // If queue is empty, block the calling thread util there is item to pop or timeout have passed.
    // Return: false on timeout , true on successful pop
    template<class Duration_Rep, class Duration_Period>
    bool pop(T& item, const std::chrono::duration<Duration_Rep, Duration_Period>& timeout)
    {
        std::unique_lock<std::mutex> ul(_mutex);
        if (_q.empty())
        {
            if (!_item_pushed_cond.wait_until(ul, clock::now() + timeout, [this]() { return !this->_q.empty(); }))
                return false;
        }
        item = _q.front();
        _q.pop();
        if (_q.size() >= _max_size - 1)
        {
            ul.unlock();
            _item_popped_cond.notify_one();
        }
        return true;
    }

    // Pop a copy of the front item in the queue into the given item ref
    // If queue is empty, block the calling thread util there is item to pop.
    void pop(T& item)
    {
        while (!pop(item, std::chrono::hours::max()));
    }

private:
    size_type _max_size;
    std::queue<T> _q;
    std::mutex _mutex;
    std::condition_variable _item_pushed_cond;
    std::condition_variable _item_popped_cond;
};
}
}