#pragma once

#include "JobRunner.h"

typedef uint32_t JobHandle;

class JobSystem
{
 public:
    virtual ~JobSystem() {}

    virtual void RunJobInstantly(const job_fp& job) = 0;
    virtual JobHandle RunJobRepeatedly(const job_fp& job, uint64_t interval_millis) = 0;

    virtual bool CancelJob(JobHandle handle) = 0;
};