#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class StorageBuffer;

	static constexpr uint32_t sLightBufferBinding = 4;
	static constexpr uint32_t sMaxLights = 32;

	struct FLightInfo
	{
		alignas(16) glm::uvec3 NumLights = {0, 0, 0};
		std::array<FDirectionalLightCreateInfo, sMaxLights> DirectionalLightInfo;
		std::array<FPointLightCreateInfo, sMaxLights> PointLightInfo;
		std::array<FSpotLightCreateInfo, sMaxLights> SpotLightInfo;
	};

	struct LightBuffer
	{
		void Init();

		void Begin();

		void End();

		void Submit(const FDirectionalLightCreateInfo &info);

		void Submit(const FPointLightCreateInfo &info);

		void Submit(const FSpotLightCreateInfo &info);

		const glm::uvec3 &NumLights() const;

	private:
		FLightInfo LightInfo{};

		Ref<StorageBuffer> LightBuffer;
	};

} // namespace BHive