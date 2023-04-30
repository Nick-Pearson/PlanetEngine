#pragma once

#include <memory>

#include "Entity/Entity.h"
#include "TimeOfDay.h"
#include "World/Worley.h"

class MeshComponent;

class SkyDome: public Entity
{
 public:
    SkyDome();
    virtual ~SkyDome();


    void OnUpdate(float deltaSeconds) override;
    void GenerateTextures();

 private:
    TimeOfDay* time_of_day_;
    Vector sun_colour_ = Vector{ 1.0f, 1.0f, 1.0f };
    float sun_sky_strength_ = 20.0f;

    WorleyParams low_freq_worley_params_;
    Worley* low_freq_worley_;
    WorleyParams high_freq_worley_params_;
    Worley* high_freq_worley_;

    uint64_t gen_time_ms_ = 0;

    // static std::unique_ptr<Texture3D> sScatteringTexture;
    // static std::unique_ptr<Texture2D> sTransmitanceTexture;
    // static std::unique_ptr<Texture2D> sPhaseTexture;
    // static std::unique_ptr<Texture1D> sAmbientTexture;
};