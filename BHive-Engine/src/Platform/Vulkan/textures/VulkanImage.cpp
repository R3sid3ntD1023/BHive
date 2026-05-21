#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/ImageViewBuilder.h"

namespace BHive
{

	void VulkanImage::Initialize(const ImageCreateInfo &info, ImageState initial)
	{
		mAspect = info.ViewCI.subresourceRange.aspectMask;

		auto mutable_info = info;
		const auto &levels = info.ImageCI.mipLevels;
		const auto &layers = info.ImageCI.arrayLayers;
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		auto reqSize = info.ImageCI.extent.width * info.ImageCI.extent.height * info.ImageCI.extent.depth * info.BytesPerPixel;
		auto image_id = gpu_r_m.CreateImage(info.ImageCI, vk::MemoryPropertyFlagBits::eDeviceLocal, reqSize, info.DebugName);
		auto sampler_id = gpu_r_m.CreateSampler(info.SamplerCI, std::format("Image_{}_Sampler", info.DebugName));

		mImage.Image = image_id;
		mImage.Sampler = sampler_id;
		mImage.ArrayLayers = layers;
		mImage.MipLevels = levels;
		mImage.DebugName = info.DebugName;
		mImage.Usage = info.ImageCI.usage;

		mStateTracker.Initialize(layers, levels, initial);
		
		auto image = mImage.GetImage();
		mutable_info.ViewCI.setImage(image);
		ImageViewBuilder::Build(mImage, mutable_info.ViewCI, info.ViewTopology);

	}

	void VulkanImage::Initialize(const vk::Image &img, const ImageCreateInfo &info, ImageState initial)
	{
		mAspect = info.ViewCI.subresourceRange.aspectMask;

		auto mutable_info = info;
		mutable_info.ViewCI.setImage(img);

		const auto& levels = info.ImageCI.mipLevels;
		const auto& layers = info.ImageCI.arrayLayers;
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		mStateTracker.Initialize(layers, levels, initial);

		ImageViewBuilder::Build(mImage, mutable_info.ViewCI, mutable_info.ViewTopology);

		auto sampler_id = gpu_r_m.CreateSampler(mutable_info.SamplerCI, std::format("Image_{}_Sampler", mutable_info.DebugName));

		mImage.Image = gpu_r_m.RegisterExternalImage(img);
		mImage.Sampler = sampler_id;
		mImage.ArrayLayers = layers;
		mImage.MipLevels = levels;
		mImage.DebugName = info.DebugName;
		mImage.Usage = info.ImageCI.usage;

		mRawImage = 1;
	}

	void VulkanImage::Upload(const void *data, size_t size, const ImageCopyRegion &region, const ImageSubresource &sub)
	{
		auto stagingInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		auto stagingID = gpu_r_m.CreateBuffer(stagingInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

		if (auto mapped = gpu_r_m.MapMemory(stagingID, 0, size))
		{
			std::memcpy(mapped, data, size);
			gpu_r_m.UnmapMemory(stagingID);
		}

		auto& staging_buffer = VulkanBackend::GetGPUResourceManager().GetBuffer(stagingID);

		SingleTimeCommand cmd{};
		Transition(cmd, ImageState::TansferDst(), sub);
		VulkanUtils::CopyBufferToImage(cmd, staging_buffer, mImage.GetImage(), region);
		Transition(cmd, ImageState::ShaderRead(), sub);

		gpu_r_m.DestroyBuffer(stagingID);
	}

	void VulkanImage::Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub)
	{
		ASSERT(mStateTracker.MipStates.size(), "Invalid layer size must be 1 or greater -> {}", mImage.DebugName);

		auto image = mImage.GetImage();
		for (uint32_t layer = sub.BaseArrayLayer; layer < sub.BaseArrayLayer + sub.LayerCount; layer++)
		{
			for (uint32_t mip = sub.MipLevel; mip < sub.MipLevel + sub.LevelCount; mip++)
			{
				ASSERT(mStateTracker.MipStates[layer].size(), "Invalid mip size must be 1 or greater -> {}", mImage.DebugName);

				auto &oldState = mStateTracker.Get(layer, mip);

				ImageSubresource layerSub = sub;
				layerSub.BaseArrayLayer = layer;
				layerSub.LayerCount = 1;
				layerSub.MipLevel = mip;
				layerSub.LevelCount = 1;

				VulkanUtils::TransitionImageLayout(cmd, image, oldState.Layout, newState.Layout, oldState.Access, newState.Access, oldState.Stage, newState.Stage, mAspect, layerSub);
				oldState = newState;
			}
		}
	}

	void VulkanImage::Destroy()
	{
		if (!mRawImage)
			VulkanBackend::GetGPUResourceManager().DestroyImage(mImage);
	}
} // namespace BHive