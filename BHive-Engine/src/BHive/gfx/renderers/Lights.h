#pragma once

#include "LightCasters.h"
#include "core/Core.h"
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

		BufferPtr GetDirectionalLightBuffer() const { return mDirectionalLightBuffer; }

		BufferPtr GetLocalLightBuffer() const { return mLocalLightBuffer; }

	private:
		std::vector<FGPUDirectionalLight> mDirectionalLights;
		std::vector<FGPULocalLight> mLocalLights;

		BufferPtr mDirectionalLightBuffer;
		BufferPtr mLocalLightBuffer;
	};

} // namespace BHive