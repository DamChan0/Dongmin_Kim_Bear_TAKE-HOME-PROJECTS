#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

template <typename T>
class CommandQueue
{
   private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;

   public:
    void push(const T& command)
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(command);
        cv.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        T command = queue.front();
        queue.pop();
        return command;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};

#endif  // COMMAND_QUEUE_H
