#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <stdexcept>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;

public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    T& front() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return queue_.front();
    }

    const T& front() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return queue_.front();
    }

    T& back() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return queue_.back();
    }

    const T& back() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return queue_.back();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    void push(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
    }

    void pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        queue_.pop();
    }

    bool try_push(const T& value) {
        if (mutex_.try_lock()) {
            queue_.push(value);
            mutex_.unlock();
            return true;
        }
        return false;
    }

    bool try_pop(T& value) {
        if (mutex_.try_lock()) {
            if (!queue_.empty()) {
                value = std::move(queue_.front());
                queue_.pop();
                mutex_.unlock();
                return true;
            }
            mutex_.unlock();
        }
        return false;
    }
};

#endif // THREAD_SAFE_QUEUE_HPP
