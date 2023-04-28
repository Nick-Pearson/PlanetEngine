#include "UpdateEvents.h"

void UpdateEvents::Trigger(float delta_time)
{
    for (auto& func : functions_)
    {
        func(delta_time);
    }
}

void UpdateEvents::AddListener(const update_fp& function)
{
    functions_.push_back(function);
}