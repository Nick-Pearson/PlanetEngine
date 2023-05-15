#include "Worley.h"

#include "Compute/ComputeShader.h"
#include "Render/RenderSystem.h"
#include "PlanetEngine.h"

Worley::Worley(const std::shared_ptr<Texture>& worley_texture) :
    worley_texture_(worley_texture)
{
    P_ASSERT(worley_texture->GetDimensions() == 3, "Worley must apply to a 3d texture")
}

void Worley::SetParams(const WorleyParams& params)
{
    params_ = params;
}

void Worley::RegenerateTexture()
{
    auto num_cells = params_.num_cells_;
    if (num_cells < 1)
    {
        return;
    }

    std::srand(params_.seed_);

    unsigned int total_cells = num_cells * num_cells * num_cells;

    points_.resize(total_cells);
    WorleyPoint val = WorleyPoint{};
    std::fill(points_.begin(), points_.end(), val);
    for (WorleyPoint& p : points_)
    {
        p.x_ = static_cast<float>(std::rand()) / RAND_MAX;
        p.y_ = static_cast<float>(std::rand()) / RAND_MAX;
        p.z_ = static_cast<float>(std::rand()) / RAND_MAX;
    }

    unsigned int thread_group_size = 8u;
    unsigned int num_thread_groups = worley_texture_->GetWidth() / thread_group_size;
    ComputeShader* worley_compute = new ComputeShader{"cs/Worley.hlsl", NumThreads{num_thread_groups, num_thread_groups, num_thread_groups}};
    worley_compute->AddTextureOutput(worley_texture_);
    worley_compute->AddDataInput(std::make_shared<DataBuffer>(points_.data(), sizeof(WorleyPoint), total_cells));
    worley_compute->AddDefine("NUM_CELLS", std::to_string(num_cells));
    worley_compute->AddDefine("OCTAVES", std::to_string(params_.octaves_));
    worley_compute->AddDefine("LACUNARITY", std::to_string(params_.lacunarity_));
    worley_compute->AddDefine("GAIN", std::to_string(params_.gain_));
    worley_compute->AddDefine("THREAD_GROUP_SIZE", std::to_string(thread_group_size));
    worley_compute->AddDefine("TEXTURE_WIDTH", std::to_string(worley_texture_->GetWidth()));
    worley_compute->AddDefine("INCLUDE_SIMPLEX", std::to_string(static_cast<int>(params_.include_simplex_)));

    PlanetEngine::Get()->GetRenderSystem()->InvokeCompute(worley_compute);
    delete worley_compute;
}