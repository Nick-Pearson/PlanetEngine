#pragma once

#include <memory>

#include "Entity/Entity.h"
#include "TimeOfDay.h"

class MeshComponent;

class SkyDome: public Entity
{
 public:
    SkyDome();

    void OnUpdate(float deltaSeconds) override;

 private:
    TimeOfDay* mTimeOfDay;
    Vector mSunColour = Vector{ 1.0f, 1.0f, 1.0f };
    float mSunSkyStrength = 12.0f;

    // static std::unique_ptr<Texture3D> sScatteringTexture;
    // static std::unique_ptr<Texture2D> sTransmitanceTexture;
    // static std::unique_ptr<Texture2D> sPhaseTexture;
    // static std::unique_ptr<Texture1D> sAmbientTexture;
};