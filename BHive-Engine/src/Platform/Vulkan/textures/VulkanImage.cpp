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
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[image = mImage](uint32_t)
			{
				auto &gpu_r_m = GPUResourceManager::Get();
				gpu_r_m.DestroyImage(image);
			});
	}

	void VulkanImage::Create(
		uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::ImageViewType viewType, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
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
		desc.Depth = 1;
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
		gpu_r_m.CreateImageView(mImage, view_desc);

		gpu_r_m.CreateSampler(mImage, samplerInfo);

		mDescriptor = vk::DescriptorImageInfo(mImage.Sampler, mImage.View, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanImage::Upload(const void *data, size_t size)
	{
		vk::raii::Buffer stagingBuffer = nullptr;
		vk::raii::DeviceMemory stagingMemory = nullptr;

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingMemory);

		void *map_memory = stagingMemory.mapMemory(0, size);
		std::memcpy(map_memory, data, size);
		stagingMemory.unmapMemory();

		Transition(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		VulkanUtils::CopyBufferToImage(stagingBuffer, mImage.Image, mWidth, mHeight);
		Transition(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanImage::Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		VulkanUtils::TransitionImageLayout(mImage.Image, oldLayout, newLayout);
	}
} // namespace BHive