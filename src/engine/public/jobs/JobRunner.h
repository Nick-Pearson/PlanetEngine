#pragma once

#include <functional>

typedef std::function<void()> job_fp;

class JobRunner
{
 public:
    virtual ~JobRunner() {}

    virtual void RunJob(const job_fp& job) = 0;
};