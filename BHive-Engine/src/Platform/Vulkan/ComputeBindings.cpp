#include "ComputeBindings.h"
#include "VulkanPipeline.h"
#include "VulkanImage.h"
#include "gfx/Texture.h"

namespace BHive
{
	FVulkanComputeBindings::FVulkanComputeBindings(VulkanPipeline* pipeline)
		: mPipeline(pipeline)
	{
		mBackendMaterial = CreateScope<VulkanBackendMaterial>();
		mBackendMaterial->Init(pipeline);
	}

	void FVulkanComputeBindings::Bind(const char *name, Ref<Texture> tex, ImageSubresourceRange range)
	{

		mBackendMaterial->BindTexture(name, tex, range.BaseMipLevel, mPipeline);
	}


	void FVulkanComputeBindings::Set(const char *name, const void *data, size_t size)
	{
		mBackendMaterial->Set(name, data, size);
	}

	void FVulkanComputeBindings::Bind(vk::CommandBuffer cmd) const
	{
		mBackendMaterial->BindImmediate(cmd, mPipeline);
	}

	/*void FVulkanComputeBindings::TransitionStorageImagesAndClear(vk::raii::CommandBuffer& cmd, vk::ClearColorValue color)
	{
		for (auto &img : mImages)
		{
			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();
			const bool isComputeImage = (img.Access == EImageAccess::ComputeStorageWrite);

			if (isComputeImage)
			{
				ImageSubresourceRange range = img.Range;
				vkImg->Transition(cmd, ImageState::ComputeWrite(), range);
				vkImg->Transition(cmd, ImageState::TransferClear(), range);

				vk::ImageSubresourceRange clearRange(vk::ImageAspectFlagBits::eColor, range.BaseMipLevel, range.LevelCount, range.BaseArrayLayer, range.LayerCount);
				cmd.clearColorImage(vkImg->Native().GetImage(), vk::ImageLayout::eGeneral, color, clearRange);

				vkImg->Transition(cmd, ImageState::ComputeWrite(), range);
			}
		}
	}*/

} // namespace BHive