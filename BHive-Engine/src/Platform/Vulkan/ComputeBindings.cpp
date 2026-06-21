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

	void FVulkanComputeBindings::StorageImage(const char *name, const FImageInfo &info)
	{
		mBackendMaterial->BindTexture(name, info.Texture, info.BaseMip, mPipeline);
		mImages.emplace_back(info);
	}

	void FVulkanComputeBindings::SampledImage(const char *name, const FImageInfo &info)
	{
		mBackendMaterial->BindTexture(name, info.Texture, info.BaseMip, mPipeline);
		mSamplers.emplace_back(info);
	}

	void FVulkanComputeBindings::Set(const char *name, const void *data, size_t size)
	{
		mBackendMaterial->Set(name, data, size);
	}

	void FVulkanComputeBindings::Bind(vk::CommandBuffer cmd) const
	{
		mBackendMaterial->BindImmediate(cmd, mPipeline);
	}

	void FVulkanComputeBindings::TransitionStorageImagesAndClear(vk::raii::CommandBuffer& cmd, vk::ClearColorValue color)
	{
		for (auto &img : mImages)
		{
			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();
			const bool isComputeImage = (img.Access == EImageAccess::WRITE || img.Access == EImageAccess::READ_WRITE);

			ImageSubresource sub{img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount};

			if (isComputeImage)
			{
				vkImg->Transition(cmd, ImageState::ComputeWrite(), sub);
				vkImg->Transition(cmd, ImageState::TansferClear(), sub);

				vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount);
				cmd.clearColorImage(vkImg->Native().GetImage(), vk::ImageLayout::eGeneral, color, range);

				vkImg->Transition(cmd, ImageState::ComputeWrite(), sub);
			}
		}
	}

	void FVulkanComputeBindings::TransitionStorageImagesToShader(vk::raii::CommandBuffer &cmd)
	{
		for (auto &img : mImages)
		{
			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();

			ImageSubresource sub{img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount};
			vkImg->Transition(cmd, ImageState::ShaderRead(), sub);
		}
	}

	void FVulkanComputeBindings::TransitionSamplerImagesToShader(vk::raii::CommandBuffer &cmd)
	{
		for (auto &img : mSamplers)
		{
			auto vkImg = img.Texture->GetNativeHandle().As<VulkanImage>();

			ImageSubresource sub{img.BaseMip, img.LevelCount, img.BaseLayer, img.LayerCount};
			vkImg->Transition(cmd, ImageState::ShaderRead(), sub);
		}
	}

} // namespace BHive