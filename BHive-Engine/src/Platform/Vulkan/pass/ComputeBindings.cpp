#include "ComputeBindings.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/material/VulkanBackendMaterial.h"

namespace BHive
{
	FVulkanComputeBindings::FVulkanComputeBindings(const Ref<VulkanPipeline> &pipeline)
		: mPipeline(pipeline)
	{
		mBackendMaterial = Cast<VulkanBackendMaterial>(IMaterialBackendInterface::Create());
		mBackendMaterial->Init(pipeline);
	}

	void FVulkanComputeBindings::StorageImage(const char *name, const FImageInfo &info)
	{
		mBackendMaterial->BindTextureImmediate(name, info.Texture, info.BaseMip, mPipeline);
		mImages.emplace_back(info, true);
	}

	void FVulkanComputeBindings::SampledImage(const char *name, const FImageInfo &info)
	{
		mBackendMaterial->BindTextureImmediate(name, info.Texture, info.BaseMip, mPipeline);
		mImages.emplace_back(info, false);
	}

	void FVulkanComputeBindings::Set(const char *name, const void *data, size_t size)
	{
		mBackendMaterial->Set(name, data, size);
	}

	void FVulkanComputeBindings::Bind() const
	{
		mBackendMaterial->Bind(mPipeline);
	}

	void FVulkanComputeBindings::BindImmediate(vk::CommandBuffer cmd) const
	{
		mBackendMaterial->BindImmediate(cmd, mPipeline);
	}
} // namespace BHive