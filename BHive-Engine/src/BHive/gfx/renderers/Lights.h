#pragma once

#include "core/Core.h"
#include "gfx/renderers/RenderData.h"

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

		void Submit(const FDirectionalLight &light);

		void Submit(const FPointLight &light);

		void Submit(const FSpotLight &light);

		const glm::uvec3 &NumLights() const;

	private:
		struct FLightBufferData
		{
			glm::uvec4 NumLights = {0, 0, 0, 0};
			std::array<FDirectionalLight, sMaxLights> DirectionalLightInfo;
			std::array<FPointLight, sMaxLights> PointLightInfo;
			std::array<FSpotLight, sMaxLights> SpotLightInfo;
		};

	private:
		FLightBufferData mLightInfo{};

		Ref<GPUBuffer> mLightBuffer;
	};

} // namespace BHive