#include "GFSDK_Aftermath.h"
#include "GFSDK_Aftermath_Defines.h"
#include "GFSDK_Aftermath_GpuCrashDump.h"
#include "GFSDK_Aftermath_GpuCrashDumpDecoding.h"
#include "GFSDK_Aftermath_GpuCrashDumpEditing.h"
#include "Platform/Vulkan/VulkanCore.h"
#include "VulkanRenderDocAPI.h"

namespace BHive
{
	namespace callbacks
	{
		void GpuCrashDumpCallback(const void *pGpuCrashDump, const uint32_t gpuCrashDumpSize, void *pUserData)
		{
			VulkanAPIDebugger *crash_tracker = reinterpret_cast<VulkanAPIDebugger *>(pUserData);
			crash_tracker->OnCrashDump(pGpuCrashDump, gpuCrashDumpSize);
		}

		void ShaderDebugInfoCallback(const void *pShaderDebugInfo, const uint32_t shaderDebugInfoSize, void *pUserData)
		{
			VulkanAPIDebugger *crash_tracker = reinterpret_cast<VulkanAPIDebugger *>(pUserData);
			crash_tracker->OnShaderDebugInfo(pShaderDebugInfo, shaderDebugInfoSize);
		}

		void CrashDumpDescriptionCallback(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription addDescription, void *pUserData)
		{
			VulkanAPIDebugger *crash_tracker = reinterpret_cast<VulkanAPIDebugger *>(pUserData);
			crash_tracker->OnCrashDumpDescription(addDescription);
		}

		void ResolveMarkerCallback(const void *pMarkerData, const uint32_t markerDataSize, void *pUserData, PFN_GFSDK_Aftermath_ResolveMarker marker)
		{
			VulkanAPIDebugger *crash_tracker = reinterpret_cast<VulkanAPIDebugger *>(pUserData);
			crash_tracker->OnResolveMarker(pMarkerData, markerDataSize, marker);
		}
	} // namespace callbacks

	namespace details
	{
		void WriteGPUCrashDumpToFile(const void *data, uint32_t size)
		{
			std::ostringstream filename;
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);
			filename << "GPUCrashDump_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".nv-gpudmp";

			std::ofstream dumpFile(filename.str(), std::ios::out | std::ios::binary);
			if (dumpFile)
			{
				dumpFile.write(reinterpret_cast<const char *>(data), size);
				dumpFile.close();
			}
		}

		void WriteShaderDebugInformationToFile(GFSDK_Aftermath_ShaderDebugInfoIdentifier pIdentifier, const void *pShaderDebugInfo, const uint32_t shaderDebugInfoSize)
		{
		}
	} // namespace details

	void VulkanAPIDebugger::Init()
	{
		GFSDK_AFTERMATH_CALL(GFSDK_Aftermath_EnableGpuCrashDumps(
			GFSDK_Aftermath_Version_API, GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan, GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default, callbacks::GpuCrashDumpCallback, nullptr,
			callbacks::CrashDumpDescriptionCallback, nullptr, this));
	}

	void VulkanAPIDebugger::OnCrashDump(const void *pGpuCrashDump, const uint32_t gpuCrashDumpSize)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		details::WriteGPUCrashDumpToFile(pGpuCrashDump, gpuCrashDumpSize);
	}

	void VulkanAPIDebugger::OnShaderDebugInfo(const void *pShaderDebugInfo, const uint32_t shaderDebugInfoSize)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		GFSDK_Aftermath_ShaderDebugInfoIdentifier identifier = {};
		GFSDK_AFTERMATH_CALL(GFSDK_Aftermath_GetShaderDebugInfoIdentifier(GFSDK_Aftermath_Version_API, pShaderDebugInfo, shaderDebugInfoSize, &identifier));

		details::WriteShaderDebugInformationToFile(identifier, pShaderDebugInfo, shaderDebugInfoSize);
	}

	void VulkanAPIDebugger::OnCrashDumpDescription(void (*addDescription)(uint32_t, const char *))
	{
		addDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName, "BHIVE");
		addDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationVersion, "v1.0");
	}

	void VulkanAPIDebugger::OnResolveMarker(const void *pMarkerData, const uint32_t markerDataSize, void (*pResolveMarker)(const void *, uint32_t))
	{
	}

} // namespace BHive