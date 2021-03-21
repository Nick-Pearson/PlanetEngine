#pragma once

#include <memory>
#include <vector>

#include "Entity/Entity.h"
#include "Texture/ComputeTexture2D.h"

struct WorleyPoint
{
    float x_, y_;
};

class Worley : public Entity
{
 public:
    explicit Worley(const std::shared_ptr<ComputeTexture2D>& worley_texture);

    void OnUpdate(float deltaSeconds) override;

 private:
    void Run();

    int num_cells_ = 8;

    int seed_ = 0;

    int octaves_ = 4;
    float lacunarity_ = 2.0f;
    float gain_ = 0.6f;

    uint64_t gen_time_ms_ = 0;

    std::shared_ptr<ComputeTexture2D> worley_texture_;
    std::vector<WorleyPoint> points_;
};