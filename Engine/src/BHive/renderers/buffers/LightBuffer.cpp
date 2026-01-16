#include "gfx/StorageBuffer.h"
#include "LightBuffer.h"

namespace BHive
{
	void LightBuffer::Init()
	{
		LightBuffer = CreateRef<StorageBuffer>(sizeof(FLightInfo));
	}

	void LightBuffer::Begin()
	{
		LightInfo.NumLights = {0, 0, 0};
	}

	void LightBuffer::End()
	{
		ASSERT(LightBuffer, "Did you forget to call init()");

		// LightBuffer->BindBufferBase(sLightBufferBinding);
		LightBuffer->SetData(&LightInfo, sizeof(FLightInfo));
	}

	void LightBuffer::Submit(const FDirectionalLightCreateInfo &info)
	{
		auto num_lights = LightInfo.NumLights[0]++ % sMaxLights;
		LightInfo.DirectionalLightInfo[num_lights] = info;
	}

	void LightBuffer::Submit(const FPointLightCreateInfo &info)
	{
		auto num_lights = LightInfo.NumLights[1]++ % sMaxLights;
		LightInfo.PointLightInfo[num_lights] = info;
	}

	void LightBuffer::Submit(const FSpotLightCreateInfo &info)
	{
		auto num_lights = LightInfo.NumLights[2]++ % sMaxLights;
		LightInfo.SpotLightInfo[num_lights] = info;
	}

	const glm::uvec3 &BHive::LightBuffer::NumLights() const
	{
		return LightInfo.NumLights;
	}
} // namespace BHive