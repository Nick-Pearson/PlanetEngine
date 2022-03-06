#include "LazyJobSystem.h"

#include <chrono>
#include <algorithm>

namespace
{
    inline uint64_t get_current_time()
    {
        const auto now = std::chrono::steady_clock::now().time_since_epoch();
        return now/std::chrono::milliseconds(1);
    }
}

LazyJobSystem::LazyJobSystem(JobRunner* job_runner) :
    job_runner_(job_runner), next_job_handle_(1)
{
}

LazyJobSystem::~LazyJobSystem()
{
}

void LazyJobSystem::RunJobs()
{
    bool changed = false;
    uint64_t curr_time = get_current_time();
    for (auto& job : active_jobs_)
    {
        if (job.next_run_time_ <= curr_time)
        {
            job_runner_->RunJob(job.job_fp_);
            job.next_run_time_ = curr_time + job.interval_;
            changed = true;
        }
        else
        {
            break;
        }
    }

    if (changed)
    {
        std::sort(active_jobs_.begin(), active_jobs_.end(), [](const ActiveJob& a, const ActiveJob& b) {
            return a.next_run_time_ - b.next_run_time_;
        });
    }
}

void LazyJobSystem::RunJobInstantly(const job_fp& job)
{
    job_runner_->RunJob(job);
}

JobHandle LazyJobSystem::RunJobRepeatedly(const job_fp& job, uint64_t interval_millis)
{
    ActiveJob job_struct;
    job_struct.job_handle_ = next_job_handle_++;
    job_struct.job_fp_ = job;
    job_struct.next_run_time_ = get_current_time() + interval_millis;
    job_struct.interval_ = interval_millis;

    active_jobs_.push_back(job_struct);
    std::sort(active_jobs_.begin(), active_jobs_.end(), [](const ActiveJob& a, const ActiveJob& b) {
        return a.next_run_time_ - b.next_run_time_;
    });

    return job_struct.job_handle_;
}

bool LazyJobSystem::CancelJob(JobHandle handle)
{
    return false;
}