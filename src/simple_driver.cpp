#include <cassert>
#include <iostream>
#include "worker.hpp"

class TestWorker : public Worker {
protected:
    void thread_function() override {
        while (!is_thread_stop) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return is_thread_awake || is_thread_stop; });

            if (is_thread_stop) break;

            while (!input_buffer.empty()) {
                std::string data = input_buffer.front();
                input_buffer.pop();
                process_data(data);
            }

            is_thread_awake = false;
        }
    }

    void process_data(const std::string data) override {
        if (operation) {
            std::string result = operation(data);
            output_buffer.push(result);
        }
    }

public:
    TestWorker() : Worker() {
        thread = std::thread(&TestWorker::thread_function, this);
    }
};

std::string test_operation(std::string input) {
    return "Processed: " + input;
}

void test_one() {
    TestWorker worker;

    // Set the operation
    FuncPtr op = test_operation;
    worker.set_operation(op);

    // Test wake_up and put_to_sleep
    assert(!worker.is_awake());
    worker.wake_up();
    assert(worker.is_awake());
    worker.put_to_sleep();
    assert(!worker.is_awake());

    // Test data processing
    worker.add_data("Test1");
    worker.add_data("Test2");
    worker.wake_up();

    // Give some time for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assert(!worker.is_empty());
    auto results = worker.get_data(2);
    assert(results.size() == 2);
    assert(results[0] == "Processed: Test1");
    assert(results[1] == "Processed: Test2");

    // Test stop and join
    worker.stop();
    worker.join();

    std::cout << "Test one passed successfully!" << std::endl;
}

int main() {
    try {
        test_one();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
