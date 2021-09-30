#pragma once

#include <vector>
#include <functional>

typedef std::function<void(float delta_time)> update_fp;

class UpdateEvents
{
 public:
    void Trigger(float delta_time);

    void AddListener(const update_fp& function);

 private:
    std::vector<update_fp> functions_;
};