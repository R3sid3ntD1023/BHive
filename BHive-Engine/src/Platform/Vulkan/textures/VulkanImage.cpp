#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace BHive
{
	VulkanImage::VulkanImage()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{

	}

	VulkanImage::~VulkanImage()
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[image = mImage](uint32_t)
			{
				auto &gpu_r_m = GPUResourceManager::Get();
				gpu_r_m.DestroyImage(image);
			});
	}

	void VulkanImage::Create(
		uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
		vk::SamplerCreateInfo samplerInfo)
	{
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mFormat = format;

		auto &gpu_r_m = GPUResourceManager::Get();

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

		mImage.Aspect = aspect;

		ImageViewDesc view_desc{};
		view_desc.Aspect = aspect;
		view_desc.Format = format;
		view_desc.Type = viewType;
		gpu_r_m.CreateImageView(mImage, view_desc);

		gpu_r_m.CreateSampler(mImage, samplerInfo);
	}

	void VulkanImage::Upload(const void *data, size_t size, const ImageCopyRegion& region, const ImageSubresource& sub)
	{
		
		vk::raii::Buffer stagingBuffer = nullptr;
		vk::raii::DeviceMemory stagingMemory = nullptr;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingMemory);

		void *map_memory = stagingMemory.mapMemory(0, size);
		std::memcpy(map_memory, data, size);
		stagingMemory.unmapMemory();

		Vulkan::ImageState transerDst{
			vk::ImageLayout::eTransferDstOptimal,
			vk::AccessFlagBits2::eTransferWrite,
			vk::PipelineStageFlagBits2::eTransfer
		};

		Vulkan::ImageState shaderRead{
			vk::ImageLayout::eShaderReadOnlyOptimal,
			vk::AccessFlagBits2::eShaderRead,
			vk::PipelineStageFlagBits2::eFragmentShader
		};

		auto cmd = VulkanUtils::BeginSingleTimeCommands();
		mImage.Transition(cmd, transerDst, sub);
		VulkanUtils::CopyBufferToImage(cmd, stagingBuffer, mImage.Image, region);
		mImage.Transition(cmd, shaderRead,sub);
		VulkanUtils::EndSingleTimeCommands(cmd);
	}

	const vk::DescriptorImageInfo VulkanImage::GetDescriptor() const
	{
		return vk::DescriptorImageInfo(mImage.Sampler, mImage.View, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

} // namespace BHive