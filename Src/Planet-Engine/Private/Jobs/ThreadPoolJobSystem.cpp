#include "ThreadPoolJobSystem.h"

#include <utility>

#include "PlanetLogging.h"

ThreadPoolJobSystem::ThreadPoolJobSystem(int num_threads) :
    pool_(num_threads)
{
    running_ = true;

    for (int i = 0; i < pool_.size(); ++i)
    {
        pool_[i] = std::thread(&ThreadPoolJobSystem::Run, this, i);
    }
}

ThreadPoolJobSystem::~ThreadPoolJobSystem()
{
    running_ = false;
    queue_condition_.notify_all();
    for (int i = 0; i < pool_.size(); ++i)
    {
        if (pool_[i].joinable())
        {
            pool_[i].join();
        }
    }
    P_LOG("Shutting down job system, {} jobs did not run", job_queue_.size());
    pool_.clear();
}

void ThreadPoolJobSystem::Run(int thread_number)
{
    P_LOG("Starting pool thread {}", thread_number);
    std::unique_lock<std::mutex> lock(lock_);

    while (running_)
    {
        queue_condition_.wait(lock, [this]{
            return (!job_queue_.empty() || !running_);
        });

        if (!job_queue_.empty())
        {
            auto func = std::move(job_queue_.front());
            job_queue_.pop();
            lock.unlock();

            P_LOG("Running job");
            func();

            lock.lock();
        }
    }

    P_LOG("Stopping pool thread {}", thread_number);
}

bool ThreadPoolJobSystem::RunJob(const job_fp& job)
{
    std::unique_lock<std::mutex> lock(lock_);
    job_queue_.push(job);
    lock.unlock();
    queue_condition_.notify_one();
    return true;
}
