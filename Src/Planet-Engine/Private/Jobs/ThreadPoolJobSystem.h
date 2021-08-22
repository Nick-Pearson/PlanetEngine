#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "Jobs/JobSystem.h"

// Runs jobs py passing them to one of the available threads
class ThreadPoolJobSystem : public JobSystem
{
 public:
    explicit ThreadPoolJobSystem(int num_threads);
    ~ThreadPoolJobSystem();

    bool RunJob(const job_fp& job) final;

 private:
    void Run(int thread_number);

    bool running_;
    std::vector<std::thread> pool_;
    std::queue<job_fp> job_queue_;
    std::condition_variable queue_condition_;
    std::mutex lock_;
};
