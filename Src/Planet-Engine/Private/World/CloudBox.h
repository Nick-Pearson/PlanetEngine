#pragma once

#include <memory>

#include "Entity/Entity.h"
#include "Texture/ComputeTexture2D.h"
#include "World/Worley.h"

class MeshComponent;

class CloudBox: public Entity
{
 public:
    CloudBox();
    virtual ~CloudBox();

    void OnUpdate(float deltaSeconds);
    void GenerateTextures();

 private:
    WorleyParams low_freq_worley_params_;
    Worley* low_freq_worley_;
    WorleyParams high_freq_worley_params_;
    Worley* high_freq_worley_;

    uint64_t gen_time_ms_ = 0;
};