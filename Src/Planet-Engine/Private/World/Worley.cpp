#include "Worley.h"

#include <chrono>

#include "Compute/ComputeShader.h"
#include "Render/RenderSystem.h"
#include "PlanetEngine.h"

#include "imgui.h"

namespace chr = std::chrono;

Worley::Worley(const std::shared_ptr<ComputeTexture2D>& worley_texture) :
    worley_texture_(worley_texture)
{
}

void Worley::OnUpdate(float deltaSeconds)
{
    ImGui::Begin("Worley");
    int step = 1;
    bool regen = ImGui::InputScalar("Seed", ImGuiDataType_U32, &seed_, &step);
    if (ImGui::Button("Random Seed"))
    {
        seed_ = static_cast<unsigned int>(std::rand());
        regen = true;
    }

    regen |= ImGui::InputInt("Num Cells", &num_cells_);
    regen |= ImGui::InputInt("Octaves", &octaves_);
    regen |= ImGui::SliderFloat("Lacunarity", &lacunarity_, 0.0f, 20.0f);
    regen |= ImGui::SliderFloat("Gain", &gain_, 0.0f, 1.0f);

    regen |= ImGui::Button("Generate Texture");

    if (regen)
    {
        Run();
    }

    ImGui::Text("Time Taken: %dms", gen_time_ms_);
    ImGui::End();
}

void Worley::Run()
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();

    if (num_cells_ < 1)
    {
        return;
    }

    std::srand(seed_);

    points_.resize(num_cells_ * num_cells_);
    WorleyPoint val = WorleyPoint{};
    std::fill(points_.begin(), points_.end(), val);
    for (WorleyPoint& p : points_)
    {
        p.x_ = static_cast<float>(std::rand()) / RAND_MAX;
        p.y_ = static_cast<float>(std::rand()) / RAND_MAX;
    }

    ComputeShader* worley_compute = new ComputeShader{"Worley.hlsl", NumThreads{32, 32, 1}};
    worley_compute->AddTextureOutput(worley_texture_);
    worley_compute->AddDataInput(std::make_shared<DataBuffer>(points_.data(), sizeof(WorleyPoint), num_cells_ * num_cells_));
    worley_compute->AddDefine("NUM_CELLS", std::to_string(num_cells_));
    worley_compute->AddDefine("OCTAVES", std::to_string(octaves_));
    worley_compute->AddDefine("LACUNARITY", std::to_string(lacunarity_));
    worley_compute->AddDefine("GAIN", std::to_string(gain_));

    PlanetEngine::Get()->GetRenderSystem()->InvokeCompute(*worley_compute);
    delete worley_compute;

    auto time = chr::high_resolution_clock::now() - start;
    gen_time_ms_ = time/chr::milliseconds(1);
}