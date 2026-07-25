#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    //delete copy/move assignment/constructors
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    //pushes a callable "command" with a the signature "void()" into the queue
    void push(T&& element);

    //copies all currently queued up commands into another local buffer and executes it using the executor
    template<typename Executor>
    void drain(Executor&& executor);

    //sleep until a command comes in or a stop is requested
    void wait(const std::stop_token& stop_token);

private:
    std::deque<T>             m_deque{};
    mutable std::mutex          m_mutex{};
    std::condition_variable_any m_cv{};
};

template<typename T>
void ThreadSafeQueue<T>::push(T&& element) {
    {   //prevent modification to the queue while we push a new command
        std::lock_guard lock_guard{m_mutex};
        m_deque.emplace_back(std::forward<T>(element));
    }
    m_cv.notify_one();
}

template<typename T>
template<typename Executor>
void ThreadSafeQueue<T>::drain(Executor&& executor) {
    std::deque<T> local_queue;
    {
        std::lock_guard lock_guard{m_mutex};
        m_deque.swap(local_queue);
    }
    for (auto& element : local_queue) {
        std::invoke(executor, element);
    }
}

template<typename T>
void ThreadSafeQueue<T>::wait(const std::stop_token &stop_token) {
    std::unique_lock unique_lock{m_mutex};
    m_cv.wait( unique_lock, stop_token, [this] { return ( !m_deque.empty() ); } );
}
