#include "ComputeBindings.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/material/VulkanBackendMaterial.h"

namespace BHive
{
	FVulkanComputeBindings::FVulkanComputeBindings(const Ref<VulkanPipeline> &pipeline)
		: mPipeline(pipeline)
	{
		mBackendMaterial = IMaterialBackendInterface::Create();
		mBackendMaterial->Init(pipeline);
	}

	void FVulkanComputeBindings::StorageImage(const char *name, const Ref<Texture> &tex, uint32_t mip)
	{
		mBackendMaterial->BindTexture(name, tex, mip, mPipeline);
	}

	void FVulkanComputeBindings::SampledImage(const char *name, const Ref<Texture> &tex, uint32_t mip)
	{
		mBackendMaterial->BindTexture(name, tex, mip, mPipeline);
	}

	void FVulkanComputeBindings::Set(const char *name, const void *data, size_t size)
	{
		mBackendMaterial->Set(name, data, size);
	}

	void FVulkanComputeBindings::Bind() const
	{
		mBackendMaterial->Bind(mPipeline);
	}
} // namespace BHive