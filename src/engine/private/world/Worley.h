#pragma once

#include <memory>
#include <vector>

#include "Entity/Entity.h"
#include "Texture/Texture.h"

struct WorleyPoint
{
    float x_, y_, z_;
    float padding;
};

struct WorleyParams
{
    int num_cells_ = 8;
    int seed_ = 0;
    int octaves_ = 4;
    float lacunarity_ = 2.0f;
    float gain_ = 0.6f;
    bool include_simplex_ = false;

    WorleyParams& operator=(const WorleyParams& other) = default;
};

class Worley
{
 public:
    explicit Worley(const std::shared_ptr<Texture>& worley_texture);

    void SetParams(const WorleyParams& params);
    void RegenerateTexture();

 private:
    WorleyParams params_;
    std::shared_ptr<Texture> worley_texture_;
    std::vector<WorleyPoint> points_;
};