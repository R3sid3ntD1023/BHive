#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanImage::~VulkanImage()
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[image = mImage](uint32_t)
			{
				VulkanBackend::GetGPUResourceManager().DestroyImage(image);
			});
	}

	void VulkanImage::Create(vk::ImageCreateFlags createFlags,
		uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, const FVulkanTextureCreateInfo& createInfo)
	{
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		ImageDesc desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Depth = depth;
		desc.ArrayLayers = createInfo.ArrayLayers;
		desc.Format = createInfo.Format;
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Tiling = vk::ImageTiling::eOptimal;
		desc.Usage = createInfo.Usage;
		desc.Type = type;
		desc.BytesPerPixel = createInfo.BytesPerPixel;
		desc.Aspect = createInfo.Aspect;

		ImageViewDesc view_desc{};
		view_desc.Format = createInfo.Format;
		view_desc.Type = viewType;

		mImage = gpu_r_m.CreateImage(createFlags, desc, view_desc);

		vk::SamplerCreateInfo sampler_info(
			{}, createInfo.MinFilter, createInfo.MagFilter, vk::SamplerMipmapMode::eLinear, createInfo.WrapMode, createInfo.WrapMode, createInfo.WrapMode, 0, 0, 1, createInfo.CompareEnabled,
			createInfo.CompareOp);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		gpu_r_m.CreateSampler(mImage, sampler_info);
	}

	void VulkanImage::Upload(const void *data, size_t size, const ImageCopyRegion &region, const ImageSubresource &sub)
	{
		AllocatedBuffer stagingBuffer{};

		BufferDesc staging_desc{};
		staging_desc.Size = size;
		staging_desc.Usage = vk::BufferUsageFlagBits::eTransferSrc;
		staging_desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		stagingBuffer = gpu_r_m.CreateBuffer(staging_desc);

		if (auto mapped = gpu_r_m.MapMemory(stagingBuffer, 0, size))
		{
			std::memcpy(mapped, data, size);
			gpu_r_m.UnmapMemory(stagingBuffer);
		}

		ImageState transerDst{vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer};

		ImageState shaderRead{vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead, vk::PipelineStageFlagBits2::eFragmentShader};

		auto cmd = VulkanUtils::BeginSingleTimeCommands();
		mImage.Transition(cmd, transerDst, sub);
		VulkanUtils::CopyBufferToImage(cmd, stagingBuffer.Buffer, mImage.GetImage(), region);
		mImage.Transition(cmd, shaderRead, sub);
		VulkanUtils::EndSingleTimeCommands(cmd);

		gpu_r_m.DestroyBuffer(stagingBuffer);
	}
} // namespace BHive