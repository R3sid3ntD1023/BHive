#include "GPUImageResource.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/VulkanUtils.h"

namespace BHive
{
	GPUImageResource::GPUImageResource(const std::string &name, const ImageInfo &info, MemoryAllocator *allocator, bool external)
		: mAllocator(allocator)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		if (!external)
		{
			Image = device.createImage(info.ImageInfo);
			Allocation = allocator->Allocate(Image, info.MemoryFlags);
			Image.bindMemory(Allocation.Memory, Allocation.Offset);
		}

		if ((info.ImageInfo.usage & vk::ImageUsageFlagBits::eSampled))
		{
			Sampler = device.createSampler(*info.SamplerInfo);
		}

		vk::ImageViewCreateInfo viewInfo = info.ViewInfo;
		if (!external)
		{
			viewInfo.image = Image;
		}

		View = device.createImageView(viewInfo);

		auto layers = info.Layers;
		auto isCube = info.IsCube;
		auto mipLevels = info.MipLevels;

		for (uint32_t layer = 0; layer < layers; layer++)
		{

			for (uint32_t mip = 0; mip < mipLevels; mip++)
			{
				//------------------------------------
				// Cube Mip View(6 faces at mip)
				//------------------------------------
				if (isCube)
				{
					vk::ImageViewCreateInfo cubeMip = viewInfo;
					cubeMip.viewType = vk::ImageViewType::eCube;

					cubeMip.subresourceRange.baseArrayLayer = 0;
					cubeMip.subresourceRange.layerCount = 6;
					cubeMip.subresourceRange.baseMipLevel = mip;
					cubeMip.subresourceRange.levelCount = 1;
					LayerMipViews.try_emplace({layer, 6, mip}, device.createImageView(cubeMip));
				}

				//-------------------------------------------
				// Face Mip Views(2D per face)
				//-------------------------------------------
				for (uint32_t face = 0; face < (isCube ? 6 : 1); face++)
				{
					vk::ImageViewCreateInfo layerMip = viewInfo;
					layerMip.viewType = vk::ImageViewType::e2D;

					if (isCube)
					{
						layerMip.subresourceRange.baseArrayLayer = face;
						layerMip.subresourceRange.layerCount = 1;
					}
					else
					{
						layerMip.subresourceRange.baseArrayLayer = layer;
						layerMip.subresourceRange.layerCount = 1;
					}

					layerMip.subresourceRange.baseMipLevel = mip;
					layerMip.subresourceRange.levelCount = 1;
					LayerMipViews.try_emplace({layer, face, mip}, device.createImageView(layerMip));
					LayerMipStates.try_emplace({layer, face, mip}, ImageState::Undefined());
				}
			}
		}

#if BHIVE_ENABLE_OBJECT_NAMES
		if (Image != VK_NULL_HANDLE)
			VulkanBackend::SetObjectName(*Image, name);
		if (Sampler != VK_NULL_HANDLE)
			VulkanBackend::SetObjectName(*Sampler, name);
#endif

		mExtent = info.ImageInfo.extent;
		mAspectMask = info.ViewInfo.subresourceRange.aspectMask;
		auto initialState = InitFromUsage(info.ImageInfo.format, info.ImageInfo.usage);
		ImageSubresourceRange range{};
		range.BaseArrayLayer = 0;
		range.LayerCount = layers;
		range.BaseMipLevel = 0;
		range.LevelCount = mipLevels;

		SingleTimeCommand cmd{};
		Transition(cmd, initialState, range);
	}

	GPUImageResource::~GPUImageResource()
	{
		if (Allocation.IsMapped)
		{
			mAllocator->UnMap(Allocation);
			Allocation.IsMapped = false;
		}
		mAllocator->Free(Allocation);
	}

	void GPUImageResource::Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange subresourceRange)
	{
		auto layers = subresourceRange.LayerCount;
		auto mipLevels = subresourceRange.LevelCount;

		for (uint32_t layer = subresourceRange.BaseArrayLayer; layer < subresourceRange.BaseArrayLayer + layers; layer++)
		{
			for (uint32_t mip = subresourceRange.BaseMipLevel; mip < subresourceRange.BaseMipLevel + mipLevels; mip++)
			{
				auto &oldState = LayerMipStates[{layer, 0, mip}];
				if (oldState == newState)
				{
					continue;
				}

				ImageSubresourceRange singleSubresourceRange;
				singleSubresourceRange.BaseArrayLayer = layer;
				singleSubresourceRange.LayerCount = 1;
				singleSubresourceRange.BaseMipLevel = mip;
				singleSubresourceRange.LevelCount = 1;

				VulkanUtils::TransitionImageLayout(
					cmd, *Image, oldState.Layout, newState.Layout, oldState.Access, newState.Access, oldState.Stage, newState.Stage, mAspectMask, singleSubresourceRange);
				oldState = newState;
			}
		}
	}

	void GPUImageResource::GenerateMipmaps(vk::CommandBuffer cmd, uint32_t layers, glm::uvec2 size)
	{
		for (uint32_t mip = 1; mip < layers; ++mip)
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
			blit.srcSubresource = {mAspectMask, mip - 1, 0, layers};
			blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.srcOffsets[1] = vk::Offset3D{int32_t(size.x), int32_t(size.y), 1};

			blit.dstSubresource = {mAspectMask, mip, 0, layers};
			blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.dstOffsets[1] = vk::Offset3D{int32_t(std::max(size.x >> 1, 1u)), int32_t(std::max(size.y >> 1, 1u)), 1};

			cmd.blitImage(*Image, vk::ImageLayout::eTransferSrcOptimal, *Image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

			size.x = std::max(size.x >> 1, 1u);
			size.y = std::max(size.y >> 1, 1u);
		}

		ImageSubresourceRange range{0, 1, 0, layers};
		Transition(cmd, ImageState::ShaderRead(), range);
	}

	ImageState GPUImageResource::InitFromUsage(vk::Format format, vk::ImageUsageFlags usage)
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

	void GPUImageResource::UploadData(vk::CommandBuffer cmd, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange subresourceRange)
	{

		auto stagingInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		auto staging = gpu_r_m.CreateBuffer(stagingInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		if (auto mapped = staging->map(0, size))
		{
			std::memcpy(mapped, data, size);
			staging->unmap();
		}

		Transition(cmd, ImageState::TransferWrite(), subresourceRange);
		VulkanUtils::CopyBufferToImage(cmd, staging->Buffer, Image, region);
		Transition(cmd, ImageState::ShaderRead(), subresourceRange);

		staging.Destroy();
	}

	ImageState GPUImageResource::GetImageState(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		return LayerMipStates.at({layer, face, mip});
	}

	vk::ImageView GPUImageResource::GetView(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		return LayerMipViews.at({layer, face, mip});
	}

} // namespace BHive