#pragma once

#include "core/Core.h"
#include "LightCasters.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class Renderer;

	struct BHIVE_API Lights
	{
		static constexpr uint32_t sMaxLights = 32;

		void Init();

		void BeginRecording();

		void EndRecording();

		void Submit(const DirectionalLight &light);

		void Submit(const PointLight &light);

		void Submit(const SpotLight &light);

		const glm::uvec3 &NumLights() const;

		BufferPtr GetBuffer() const { return mLightBuffer; }

	private:
		struct FLightBufferData
		{
			glm::uvec4 NumLights = {0, 0, 0, 0};
			std::array<FGPUDirectionalLight, sMaxLights> DirectionalLightInfo;
			std::array<FGPUPointLight, sMaxLights> PointLightInfo;
			std::array<FGPUSpotLight, sMaxLights> SpotLightInfo;
		};

	private:
		FLightBufferData mLightInfo{};

		BufferPtr mLightBuffer;
	};

} // namespace BHive