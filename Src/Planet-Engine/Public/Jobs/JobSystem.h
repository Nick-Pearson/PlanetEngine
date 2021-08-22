#pragma once

#include <functional>

typedef std::function<void()> job_fp;

class JobSystem
{
 public:
    virtual ~JobSystem() {}

    virtual bool RunJob(const job_fp& job) = 0;
};