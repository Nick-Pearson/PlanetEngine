#pragma once

#include <memory>

#include "Entity/Entity.h"

class MeshComponent;

class SkyDome: public Entity
{
 public:
    SkyDome();

    void OnUpdate(float deltaSeconds) override;

 private:
    std::shared_ptr<MeshComponent> mDomeMesh;

    bool mPauseTime = true;
    float mCurrentTimeOfDay = 0.82f;
    Vector mSunColour = Vector{ 1.0f, 1.0f, 1.0f };
    float mSunSkyStrength = 20.0f;

    float mDayLength = 500.0f;

    // static std::unique_ptr<Texture3D> sScatteringTexture;
    // static std::unique_ptr<Texture2D> sTransmitanceTexture;
    // static std::unique_ptr<Texture2D> sPhaseTexture;
    // static std::unique_ptr<Texture1D> sAmbientTexture;
};