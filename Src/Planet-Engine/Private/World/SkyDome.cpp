#include "SkyDome.h"

#include <chrono>

#include "PlanetEngine.h"
#include "Mesh/MeshComponent.h"
#include "Material/Material.h"
#include "../Mesh/Primitives.h"
#include "Render/Renderer.h"
#include "Shader/PixelShader.h"

#include "imgui.h"

namespace chr = std::chrono;

SkyDome::SkyDome()
{
    auto mesh = Primitives::IcoHemisphere(Elipsoid(4900.0f), 3);
    mesh->FlipFaces();

    auto sky_shader = new PixelShader("SkySphere.hlsl");
    sky_shader->AddInput(ShaderParameterType::TEXTURE_3D);
    sky_shader->AddInput(ShaderParameterType::TEXTURE_3D);

    auto sky_material = std::make_shared<Material>(sky_shader);

    auto domeMesh = AddComponent<MeshComponent>(mesh, sky_material);
    domeMesh->render_config_.use_world_matrix_ = false;

    auto low_freq_texture = std::make_shared<ComputeTexture3D>(128, 128, 128);
    auto high_freq_texture = std::make_shared<ComputeTexture3D>(32, 32, 32);

    sky_material->AddTexture(low_freq_texture);
    sky_material->AddTexture(high_freq_texture);

    time_of_day_ = new TimeOfDay{};
    low_freq_worley_ = new Worley{low_freq_texture};
    high_freq_worley_ = new Worley{high_freq_texture};
    low_freq_worley_params_.include_simplex_ = true;
    high_freq_worley_params_.num_cells_ = 10;
    high_freq_worley_params_.octaves_ = 1;
    GenerateTextures();
}

SkyDome::~SkyDome()
{
    delete time_of_day_;
    delete low_freq_worley_;
    delete high_freq_worley_;
}

void SkyDome::OnUpdate(float deltaSeconds)
{
    Entity::OnUpdate(deltaSeconds);

    time_of_day_->Update(deltaSeconds);

    Vector sunDirection = time_of_day_->CalculateSunDirection();

    auto renderer = PlanetEngine::Get()->GetRenderer();
    renderer->UpdateWorldBuffer(WorldBufferData(sunDirection, sun_sky_strength_, sun_colour_));

    ImGui::Begin("Clouds");

    bool regen = false;
    if (ImGui::Button("Random Seed"))
    {
        low_freq_worley_params_.seed_ = static_cast<unsigned int>(std::rand());
        high_freq_worley_params_.seed_ = static_cast<unsigned int>(std::rand());
        regen = true;
    }

    ImGui::Text("Low Frequency Seed: %d", low_freq_worley_params_.seed_);
    ImGui::Text("Low Frequency Seed: %d", high_freq_worley_params_.seed_);
    regen |= ImGui::Button("Generate Textures");

    if (regen)
    {
        GenerateTextures();
    }

    ImGui::Text("Time Taken: %dms", gen_time_ms_);
    ImGui::End();
}

void SkyDome::GenerateTextures()
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();

    low_freq_worley_->SetParams(low_freq_worley_params_);
    low_freq_worley_->RegenerateTexture();

    high_freq_worley_->SetParams(high_freq_worley_params_);
    high_freq_worley_->RegenerateTexture();

    auto time = chr::high_resolution_clock::now() - start;
    gen_time_ms_ = time/chr::milliseconds(1);
}