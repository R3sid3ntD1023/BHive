#pragma once

#include "core/Core.h"
#include "LightCasters.h"

namespace BHive
{
	class GPUBuffer;
	class Renderer;

	struct BHIVE_API Lights
	{
		static constexpr uint32_t sMaxLights = 32;

		void Initialize(Renderer& renderer);

		void BeginRecording();

		void Flush();

		void Submit(const DirectionalLight &light);

		void Submit(const PointLight &light);

		void Submit(const SpotLight &light);

		const glm::uvec3 &NumLights() const;

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

		Ref<GPUBuffer> mLightBuffer;
	};

} // namespace BHive