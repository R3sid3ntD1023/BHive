#include "GPUResourceHandle.h"
#include "GPUResourceManager.h"
#include "VulkanBackend.h"

namespace BHive
{
	void GPUBufferResourceHandle::Destroy()
	{
		GPUResourceHandle::Destroy();
		VulkanBackend::GetGPUResourceManager().Destroy(*this);
	}

	void GPUImageResourceHandle::Destroy()
	{
		GPUResourceHandle::Destroy();
		// VulkanBackend::GetGPUResourceManager().Destroy(*this);
	}

	GPUBufferResource *GPUBufferResourceHandle::Resolve()
	{
		return VulkanBackend::Get().GetGPUResourceManager().ResolveBuffer(*this);
	}

	GPUImageResource *GPUImageResourceHandle::Resolve()
	{
		return VulkanBackend::Get().GetGPUResourceManager().ResolveImage(*this);
	}
} // namespace BHive