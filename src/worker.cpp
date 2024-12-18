#include "worker.hpp"

Worker::Worker() : is_thread_awake(false), is_thread_stop(false) {}

Worker::~Worker() {
    if (thread.joinable()) {
        stop();
        thread.join();
    }
}

void Worker::set_operation(FuncPtr& funcptr) {
    operation = funcptr;
}

void Worker::wake_up() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        is_thread_awake = true;
    }
    cv.notify_one();
}

void Worker::put_to_sleep() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        is_thread_awake = false;
    }
    cv.notify_one();
}

void Worker::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        is_thread_stop = true;
    }
    cv.notify_one();
}

bool Worker::is_empty() {
    return !is_thread_awake && \
           input_buffer.empty() && \
           output_buffer.empty();
}

bool Worker::is_awake() {
    return is_thread_awake;
}

void Worker::join() {
    thread.join();
}

void Worker::add_data(const std::string data) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        input_buffer.push(data);
    }
    cv.notify_one();
}

std::vector<std::string> Worker::get_data(size_t size) {
    std::vector<std::string> processed_data;
    std::lock_guard<std::mutex> lock(mutex);
    
    while (!output_buffer.empty() && processed_data.size() < size) {
        processed_data.push_back(std::move(output_buffer.front()));
        output_buffer.pop();
    }
    
    return processed_data;
}
