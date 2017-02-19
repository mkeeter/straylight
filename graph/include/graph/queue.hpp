#pragma once

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

template<typename T>
class shared_queue
{
public:
    void push(T item){
        std::lock_guard<std::mutex> lock(mut);
        queue.push(item);
        cond.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mut);
        cond.wait(lock, [this]{ return !queue.empty(); });
    }

    T pop() {
        std::lock_guard<std::mutex> lock(mut);
        assert(!queue.empty());
        auto popped = queue.front();
        queue.pop();
        return popped;
    }

    const T& peek() {
        std::lock_guard<std::mutex> lock(mut);
        assert(!queue.empty());
        return queue.front();
    }

    bool empty() const{
        std::lock_guard<std::mutex> lock(mut);
        return queue.empty();
    }

    unsigned size() const{
        std::lock_guard<std::mutex> lock(mut);
        return queue.size();
    }

protected:
    std::queue<T> queue;
    mutable std::mutex mut;
    std::condition_variable cond;
};
