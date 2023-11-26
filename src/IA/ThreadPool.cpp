//
// Created by Theophilus Homawoo on 26/11/2023.
//

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads):
    _stop(false),
    _numThreads(numThreads)
{
    for (size_t i = 0; i < numThreads; ++i) {
        _workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->_queue_mutex);
                    this->_condition.wait(lock, [this] {
                        return this->_stop || !this->_tasks.empty();
                    });
                    if (this->_stop && this->_tasks.empty()) {
                        return;
                    }
                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _stop = true;
    }
    _condition.notify_all();
    for (std::thread &worker : _workers) {
        worker.join();
    }
}

std::size_t ThreadPool::getNumThreads() const
{
    return _numThreads;
}
