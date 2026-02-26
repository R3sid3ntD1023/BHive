#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanUtils.h"

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

	void VulkanImage::Create(
		uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
		vk::SamplerCreateInfo samplerInfo)
	{
		mFormat = format;

		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		ImageDesc desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Depth = depth;
		desc.ArrayLayers = layers;
		desc.Format = format;
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Tiling = vk::ImageTiling::eOptimal;
		desc.Usage = usage;
		desc.Type = type;

		mImage = gpu_r_m.CreateImage(desc);

		ImageViewDesc view_desc{};
		view_desc.Aspect = aspect;
		view_desc.Format = format;
		view_desc.Type = viewType;
		view_desc.ArrayLayers = layers;
		gpu_r_m.CreateImageView(mImage, view_desc);

		gpu_r_m.CreateSampler(mImage, samplerInfo);
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

	const vk::DescriptorImageInfo VulkanImage::GetDescriptor() const
	{
		return mImage.GetDescriptor();
	}

} // namespace BHive