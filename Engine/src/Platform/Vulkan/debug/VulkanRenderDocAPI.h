#pragma once

#include "gfx/debug/RenderDoc.h"

namespace BHive
{
	class BHIVE_API VulkanAPIDebugger : public APIDebugger
	{
	public:
		virtual void Init() override;

		void OnCrashDump(const void *pGpuCrashDump, const uint32_t gpuCrashDumpSize);

		void OnShaderDebugInfo(const void *pShaderDebugInfo, const uint32_t shaderDebugInfoSize);

		void OnCrashDumpDescription(void (*addDescription)(uint32_t, const char *));

		void OnResolveMarker(const void *pMarkerData, const uint32_t markerDataSize, void (*pResolveMarker)(const void *, uint32_t));

	private:
		std::mutex mMutex;
	};
} // namespace BHive