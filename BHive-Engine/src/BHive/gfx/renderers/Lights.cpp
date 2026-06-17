#include "Lights.h"
#include "gfx/Buffers.h"
#include "Renderer.h"

namespace BHive
{
	void Lights::Initialize(Renderer& renderer)
	{
		mLightBuffer = GPUBuffer::Create(sizeof(FLightBufferData), EBufferType::StorageBuffer);
		renderer.GetGlobalResources().Register("Lights", mLightBuffer);
	}

	void Lights::BeginRecording()
	{
		mLightInfo.NumLights = {0, 0, 0, 0};
	}

	void Lights::Flush()
	{
		mLightBuffer->SetData(&mLightInfo, sizeof(FLightBufferData));
	}

	void Lights::Submit(const FDirectionalLight &light)
	{
		auto num_lights = mLightInfo.NumLights[0]++ % sMaxLights;
		mLightInfo.DirectionalLightInfo[num_lights] = light;
	}

	void Lights::Submit(const FPointLight &light)
	{
		auto num_lights = mLightInfo.NumLights[1]++ % sMaxLights;
		mLightInfo.PointLightInfo[num_lights] = light;
	}

	void Lights::Submit(const FSpotLight &light)
	{
		auto num_lights = mLightInfo.NumLights[2]++ % sMaxLights;
		mLightInfo.SpotLightInfo[num_lights] = light;
	}

	const glm::uvec3 &BHive::Lights::NumLights() const
	{
		return mLightInfo.NumLights;
	}
} // namespace BHive