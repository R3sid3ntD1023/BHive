#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/ImageViewBuilder.h"

namespace BHive
{
	VulkanImage::~VulkanImage()
	{
		if (OnDestroyed)
			OnDestroyed(mImage.Image);
	}

	void VulkanImage::Initialize(const ImageCreateInfo &info)
	{
		mInfo = info;

		auto mutable_info = info;
		const auto &levels = info.ImageCI.mipLevels;
		const auto &layers = info.ImageCI.arrayLayers;
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		auto image_id = gpu_r_m.CreateImage(info.ImageCI, vk::MemoryPropertyFlagBits::eDeviceLocal, info.DebugName);

		mImage.Image = image_id;
		mImage.DebugName = info.DebugName;
		mImage.Usage = info.ImageCI.usage;
		mImage.IsCube = info.ViewTopology == EViewTopology::Cube;
		mImage.IsCubeArray = info.ViewTopology == EViewTopology::CubeArray;

		mStateTracker.Initialize(layers, levels, ImageState::Undefined());

		if (info.ImageCI.usage & vk::ImageUsageFlagBits::eSampled)
		{
			auto sampler_id = gpu_r_m.CreateSampler(mutable_info.SamplerCI, std::format("Image_{}_Sampler", mutable_info.DebugName));
			mImage.Sampler = sampler_id;
		}

		auto image = mImage.GetImage();
		mutable_info.ViewCI.setImage(image);

		ImageViewBuildInfo build_info{.Layers = layers, .Levels = levels, .ViewCI = mutable_info.ViewCI, .DebugName = info.DebugName};
		ImageViewBuilder::Build(mImage, build_info, info.ViewTopology);

		auto initialState = InitialStateFromUsage(info.ImageCI.usage, info.ImageCI.format);
		if (!initialState.IsUndefined)
		{
			SingleTimeCommand cmd{};
			ImageSubresourceRange fullRange{0, levels, 0, layers};
			Transition(cmd, initialState, fullRange);
		}
	}

	void VulkanImage::Initialize(const vk::Image &img, const ImageCreateInfo &info)
	{
		mInfo = info;

		auto mutable_info = info;
		mutable_info.ViewCI.setImage(img);

		const auto &levels = info.ImageCI.mipLevels;
		const auto &layers = info.ImageCI.arrayLayers;
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		mStateTracker.Initialize(layers, levels, ImageState::Undefined());

		ImageViewBuildInfo build_info{.Layers = layers, .Levels = levels, .ViewCI = mutable_info.ViewCI, .DebugName = info.DebugName};
		ImageViewBuilder::Build(mImage, build_info, mutable_info.ViewTopology);

		if (info.ImageCI.usage & vk::ImageUsageFlagBits::eSampled)
		{
			auto sampler_id = gpu_r_m.CreateSampler(mutable_info.SamplerCI, std::format("Image_{}_Sampler", mutable_info.DebugName));
			mImage.Sampler = sampler_id;
		}

		mImage.Image = gpu_r_m.RegisterExternalImage(img);
		mImage.DebugName = info.DebugName;
		mImage.Usage = info.ImageCI.usage;
	}

	void VulkanImage::Upload(const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range)
	{
		auto stagingInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		auto stagingID = gpu_r_m.CreateBuffer(stagingInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		if (auto mapped = gpu_r_m.MapMemory(stagingID, 0, size))
		{
			std::memcpy(mapped, data, size);
			gpu_r_m.UnmapMemory(stagingID);
		}

		auto staging_buffer = VulkanBackend::GetGPUResourceManager().GetBuffer(stagingID);

		SingleTimeCommand cmd{};
		Transition(cmd, ImageState::TransferWrite(), range);
		VulkanUtils::CopyBufferToImage(cmd, staging_buffer, mImage.GetImage(), region);
		Transition(cmd, ImageState::ShaderRead(), range);

		gpu_r_m.DestroyBuffer(stagingID);
	}

	void VulkanImage::Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange range)
	{
		ASSERT(mStateTracker.MipStates.size(), "Invalid layer size must be 1 or greater -> {}", mImage.DebugName);

		auto image = mImage.GetImage();
		auto aspect = mInfo.ViewCI.subresourceRange.aspectMask;

		for (uint32_t layer = range.BaseArrayLayer; layer < range.BaseArrayLayer + range.LayerCount; layer++)
		{
			for (uint32_t mip = range.BaseMipLevel; mip < range.BaseMipLevel + range.LevelCount; mip++)
			{
				ASSERT(mStateTracker.MipStates[layer].size(), "Invalid mip size must be 1 or greater -> {}", mImage.DebugName);

				auto &oldState = mStateTracker.Get(layer, mip);
				auto oldLayout = oldState.IsUndefined ? vk::ImageLayout::eUndefined : oldState.Layout;
				auto oldAccess = oldState.IsUndefined ? vk::AccessFlagBits2{} : oldState.Access;
				auto oldStage = oldState.IsUndefined ? vk::PipelineStageFlagBits2::eTopOfPipe : oldState.Stage;

				if (oldLayout == newState.Layout && oldAccess == newState.Access && oldStage == newState.Stage)
				{
					oldState.IsUndefined = false;
					continue;
				}

				ImageSubresourceRange layerSub = range;
				layerSub.BaseArrayLayer = layer;
				layerSub.LayerCount = 1;
				layerSub.BaseMipLevel = mip;
				layerSub.LevelCount = 1;

				VulkanUtils::TransitionImageLayout(cmd, image, oldLayout, newState.Layout, oldAccess, newState.Access, oldStage, newState.Stage, aspect, layerSub);

				oldState = newState;
				oldState.IsUndefined = false;
			}
		}
	}

	void VulkanImage::GenerateMipMaps(vk::CommandBuffer cmd)
	{
		auto w = mInfo.ImageCI.extent.width;
		auto h = mInfo.ImageCI.extent.height;
		auto layers = mInfo.ImageCI.arrayLayers;
		auto levels = mInfo.ImageCI.mipLevels;

		vk::Image image = mImage.GetImage();

		for (uint32_t mip = 1; mip < levels; ++mip)
		{
			{
				ImageSubresourceRange range{mip - 1, 1, 0, layers};
				Transition(cmd, ImageState::TransferRead(), range);
			}

			{
				ImageSubresourceRange range{mip, 1, 0, layers};
				Transition(cmd, ImageState::TransferWrite(), range);
			}

			// Blit mip -1 -> mip
			vk::ImageBlit blit{};
			blit.srcSubresource = {vk::ImageAspectFlagBits::eColor, mip - 1, 0, layers};
			blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.srcOffsets[1] = vk::Offset3D{int32_t(w), int32_t(h), 1};

			blit.dstSubresource = {vk::ImageAspectFlagBits::eColor, mip, 0, layers};
			blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.dstOffsets[1] = vk::Offset3D{int32_t(std::max(w >> 1, 1u)), int32_t(std::max(h >> 1, 1u)), 1};

			cmd.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

			w = std::max(w >> 1, 1u);
			h = std::max(h >> 1, 1u);
		}

		ImageSubresourceRange range{0, levels, 0, layers};
		Transition(cmd, ImageState::ShaderRead(), range);
	}

	ImageState VulkanImage::GetState(uint32_t mip, uint32_t layer) const
	{
		return mStateTracker.Get(layer, mip);
	}

	void VulkanImage::DebugPrintState()
	{
		for (uint32_t layer = 0; layer < mStateTracker.MipStates.size(); layer++)
		{
			for (uint32_t mip = 0; mip < mStateTracker.MipStates[layer].size(); mip++)
			{
				LOG_TRACE("\t\t{} [layer={} mip={}] = {}", mImage.DebugName, layer, mip, vk::to_string(mStateTracker.Get(layer, mip).Layout));
			}
		}
	}

	ImageState VulkanImage::InitialStateFromUsage(vk::ImageUsageFlags usage, vk::Format format)
	{
		const bool isDepth = format == vk::Format::eD32Sfloat || format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint || format == vk::Format::eD16Unorm;

		// Depth/stencil images

		if (usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
			return isDepth ? ImageState::DepthStencilAttachment() : ImageState::ColorAttachment();

		if ((usage & vk::ImageUsageFlagBits::eColorAttachment) && !(usage & vk::ImageUsageFlagBits::eSampled))
		{
			return ImageState::ColorAttachment();
		}

		if ((usage & vk::ImageUsageFlagBits::eColorAttachment) && (usage & vk::ImageUsageFlagBits::eSampled))
		{
			return ImageState::ShaderRead();
		}

		if (usage & vk::ImageUsageFlagBits::eStorage)
		{
			return ImageState::ComputeWrite();
		}

		if (usage & vk::ImageUsageFlagBits::eTransferDst)
		{
			return ImageState::TransferWrite();
		}

		if (usage & vk::ImageUsageFlagBits::eTransferSrc)
		{
			return ImageState::TransferRead();
		}

		// fallback: undefined (rare)
		return ImageState::Undefined();
	}
} // namespace BHive