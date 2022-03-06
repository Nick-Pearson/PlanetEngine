#pragma once

#include <vector>
#include <atomic>

#include "Jobs/JobRunner.h"
#include "Jobs/JobSystem.h"

struct ActiveJob
{
    JobHandle job_handle_;
    job_fp job_fp_;
    uint64_t next_run_time_;
    uint64_t interval_;
};

static_assert(std::atomic<JobHandle>::is_always_lock_free, "JobHandle should be lock free");

class LazyJobSystem : public JobSystem
{
 public:
    explicit LazyJobSystem(JobRunner* job_runner);
    virtual ~LazyJobSystem();

    void RunJobs();

    void RunJobInstantly(const job_fp& job) override;
    JobHandle RunJobRepeatedly(const job_fp& job, uint64_t interval_millis) override;

    bool CancelJob(JobHandle handle) override;

 private:
    JobRunner* job_runner_;
    std::atomic<JobHandle> next_job_handle_;
    std::vector<ActiveJob> active_jobs_;
};