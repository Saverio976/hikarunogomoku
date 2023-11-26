//
// Created by Theophilus Homawoo on 26/11/2023.
//

#pragma once

#include <cstddef>
#include <functional>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

class ThreadPool {
    public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _tasks.emplace([task](){ (*task)(); });
        }
        _condition.notify_one();
        return res;
    }

    std::size_t getNumThreads() const;

private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;

    std::mutex _queue_mutex;
    std::condition_variable _condition;
    bool _stop;

    std::size_t _numThreads;
};
