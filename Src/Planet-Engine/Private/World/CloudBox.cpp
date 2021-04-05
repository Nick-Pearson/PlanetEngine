#include "CloudBox.h"

#include <chrono>

#include "PlanetEngine.h"
#include "Mesh/MeshComponent.h"
#include "Material/Material.h"
#include "../Mesh/Primitives.h"
#include "World/Worley.h"
#include "Texture/ComputeTexture3D.h"

#include "imgui.h"

namespace chr = std::chrono;

CloudBox::CloudBox()
{
    auto low_freq_texture = std::make_shared<ComputeTexture3D>(128, 128, 128);
    auto high_freq_texture = std::make_shared<ComputeTexture3D>(32, 32, 32);

    auto cloudMaterial = std::make_shared<Material>("CloudsShader.hlsl");
    cloudMaterial->EnableAlphaBlending();
    cloudMaterial->AddTexture(low_freq_texture);
    cloudMaterial->AddTexture(high_freq_texture);

    auto mesh = Primitives::Cube(1.0f);
    // mesh->Scale(Vector{50.0f, 1.0f, 50.0f});
    auto cloudMesh = AddComponent<MeshComponent>(mesh, cloudMaterial);
    cloudMesh->render_config_.use_depth_buffer = false;
    cloudMesh->render_config_.use_world_matrix = false;
    Translate(Vector{0.0f, -4.0f, 0.0f});

    low_freq_worley_ = new Worley{low_freq_texture};
    high_freq_worley_ = new Worley{high_freq_texture};
    low_freq_worley_params_.include_simplex_ = true;
    high_freq_worley_params_.num_cells_ = 10;
    high_freq_worley_params_.octaves_ = 1;
    GenerateTextures();
}

CloudBox::~CloudBox()
{
    delete low_freq_worley_;
    delete high_freq_worley_;
}

void CloudBox::OnUpdate(float deltaSeconds)
{
    ImGui::Begin("Clouds");

    bool regen = false;
    if (ImGui::Button("Random Seed"))
    {
        low_freq_worley_params_.seed_ = static_cast<unsigned int>(std::rand());
        high_freq_worley_params_.seed_ = static_cast<unsigned int>(std::rand());
        regen = true;
    }

    ImGui::Text("Low Frequency Seed: %d", low_freq_worley_params_.seed_);
    regen |= ImGui::InputInt("Cells (low freq)", &low_freq_worley_params_.num_cells_);
    regen |= ImGui::InputInt("Octaves (low freq)", &low_freq_worley_params_.octaves_);
    regen |= ImGui::SliderFloat("Lacunarity (low freq)", &low_freq_worley_params_.lacunarity_, 0.0f, 5.0f);
    regen |= ImGui::SliderFloat("Gain (low freq)", &low_freq_worley_params_.gain_, 0.0f, 1.0f);

    ImGui::Text("Low Frequency Seed: %d", high_freq_worley_params_.seed_);
    regen |= ImGui::InputInt("Cells (high freq)", &high_freq_worley_params_.num_cells_);
    regen |= ImGui::InputInt("Octaves (high freq)", &high_freq_worley_params_.octaves_);
    regen |= ImGui::SliderFloat("Lacunarity (high freq)", &high_freq_worley_params_.lacunarity_, 0.0f, 5.0f);
    regen |= ImGui::SliderFloat("Gain (high freq)", &high_freq_worley_params_.gain_, 0.0f, 1.0f);

    regen |= ImGui::Button("Generate Textures");

    if (regen)
    {
        GenerateTextures();
    }

    ImGui::Text("Time Taken: %dms", gen_time_ms_);
    ImGui::End();
}

void CloudBox::GenerateTextures()
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();

    low_freq_worley_->SetParams(low_freq_worley_params_);
    low_freq_worley_->RegenerateTexture();

    high_freq_worley_->SetParams(high_freq_worley_params_);
    high_freq_worley_->RegenerateTexture();

    auto time = chr::high_resolution_clock::now() - start;
    gen_time_ms_ = time/chr::milliseconds(1);
}
