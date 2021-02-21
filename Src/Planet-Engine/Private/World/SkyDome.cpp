#include "SkyDome.h"
#include "PlanetEngine.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"
#include "../Renderer/D3D11/D3DRenderer.h"
#include "../Renderer/RenderManager.h"
#include "imgui.h"
#include "../Material/Material.h"

SkyDome::SkyDome()
{
	std::shared_ptr<Mesh> mesh = Primitives::SubdivisionSurfacesElipsoid(Elipsoid(1.0f), 5);
	mesh->FlipFaces();
	std::shared_ptr<Material> skyMaterial = std::make_shared<Material>("SkySphere.hlsl");
	mDomeMesh = AddComponent<MeshComponent>(mesh, skyMaterial);
	mDomeMesh->SetUseDepthBuffer(false);
	mDomeMesh->SetUseWorldMatrix(false);
}

void SkyDome::OnUpdate(float deltaSeconds)
{
	Entity::OnUpdate(deltaSeconds);

	if (!mPauseTime)
	{
		mCurrentTimeOfDay += deltaSeconds / mDayLength;
		if (mCurrentTimeOfDay > 1.0f)
		{
			mCurrentTimeOfDay -= 1.0f;
		}
	}

	Quaternion sunRotation{ Vector(mCurrentTimeOfDay * 360.0f, 0.0f, 0.0f) };

	PlanetEngine::Get()->GetRenderManager()->GetRenderer()->UpdateWorldBuffer(WorldBufferData(sunRotation * Vector{0.0f, 1.0f, 0.0f}, mSunSkyStrength, mSunColour));

	ImGui::Begin("Sky Dome");
	ImGui::SliderFloat("Sun Strength", &mSunSkyStrength, 0.0f, 50.0f, "%.2f strength");
	ImGui::Checkbox("Pause Time", &mPauseTime);
	ImGui::SliderFloat("Time of Day", &mCurrentTimeOfDay, 0.0f, 1.0f, "%.2f");
	ImGui::End();
}
