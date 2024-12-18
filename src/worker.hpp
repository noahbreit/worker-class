#ifndef WORKER_HPP
#define WORKER_HPP

#include "safe_queue.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <functional>

// this class expects all operations to be of type
// std::string OPERATION(std::string) { /* ... */ }
using FuncPtr = std::function<std::string(std::string)>;

class Worker {
protected:
    std::thread thread;
    std::condition_variable cv;
    std::mutex mutex;
    ThreadSafeQueue<std::string> input_buffer;
    ThreadSafeQueue<std::string> output_buffer;
    bool is_thread_awake;
    bool is_thread_stop;
    FuncPtr operation;

    virtual void thread_function() = 0;
    virtual void process_data(const std::string data) = 0;

public:
    // init
    Worker();
    virtual ~Worker();
    void set_operation(FuncPtr& funcptr);

    // control
    void wake_up();
    void put_to_sleep();
    void stop();
    void join();

    // status
    bool is_awake();
    bool is_empty();

    // data i/o
    void add_data(const std::string data);
    std::vector<std::string> get_data(size_t size = 1);
};

#endif // WORKER_HPP