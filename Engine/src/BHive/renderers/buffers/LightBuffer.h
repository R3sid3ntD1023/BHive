#pragma once

#include "core/Core.h"

namespace BHive
{
	class StorageBuffer;

	static constexpr uint32_t sLightBufferBinding = 4;
	static constexpr uint32_t sMaxLights = 32;

	struct FDirectionalLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Direction;
	};

	struct FPointLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		float Radius;
	};

	struct FSpotLightInfo
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Position;
		alignas(16) glm::vec3 Direction;
		float Radius;
		float InnerCutoff;
		float OuterCutoff;
	};

	struct FLightInfo
	{
		alignas(16) glm::uvec3 NumLights = {0, 0, 0};
		std::array<FDirectionalLightInfo, sMaxLights> DirectionalLightInfo;
		std::array<FPointLightInfo, sMaxLights> PointLightInfo;
		std::array<FSpotLightInfo, sMaxLights> SpotLightInfo;
	};

	struct LightBuffer
	{
		void Init();

		void Begin();

		void End();

		void Submit(const FDirectionalLightInfo &info);

		void Submit(const FPointLightInfo &info);

		void Submit(const FSpotLightInfo &info);

		const glm::uvec3 &NumLights() const;

	private:
		FLightInfo LightInfo{};

		Ref<StorageBuffer> LightBuffer;
	};

} // namespace BHive