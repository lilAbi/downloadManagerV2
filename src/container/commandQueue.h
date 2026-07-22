#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>

class CommandQueue {
public:
    CommandQueue() = default;
    ~CommandQueue() = default;

    //delete copy/move assignment/constructors
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue(CommandQueue&&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;
    CommandQueue& operator=(CommandQueue&&) = delete;

    //pushes a callable "command" with a the signature "void()" into the queue
    template<typename Function> requires std::invocable< std::decay_t<Function>& >
    void push(Function&& command);

    //copies all currently queued up commands into another local buffer and executes it using the executor
    template<typename Executor>
    void drain(Executor&& executor);

    //sleep until a command comes in or a stop is requested
    void wait(const std::stop_token& stop_token);

private:
    std::deque< std::move_only_function<void()> >   m_deque{};
    mutable std::mutex                              m_mutex{};
    std::condition_variable_any                     m_cv{};
};

template<typename Function> requires std::invocable< std::decay_t<Function>& >
void CommandQueue::push(Function&& command) {
    {   //prevent modification to the queue while we push a new command
        std::lock_guard lock_guard{m_mutex};
        m_deque.emplace_back(std::forward<Function>(command));
    }
    m_cv.notify_all();
}

template<typename Executor>
void CommandQueue::drain(Executor&& executor) {
    std::deque< std::move_only_function<void()> > local_queue;
    {
        std::lock_guard lock_guard{m_mutex};
        m_deque.swap(local_queue);
    }
    for (auto& command : local_queue) {
        std::invoke(executor, command);
    }
}


