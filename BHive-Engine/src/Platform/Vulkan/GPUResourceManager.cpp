#include "GPUResourceManager.h"
#include "VulkanUtils.h"

namespace BHive
{
	BufferHandle GPUResourceManager::CreateBuffer(const BufferDesc &desc)
	{
		auto handle = BufferHandle();
		auto& buffer = mBuffers[handle];
		VulkanUtils::CreateBuffer(desc.Size, desc.Usage, desc.MemoryFlags, buffer);

		return handle;
	}

	ImageHandle GPUResourceManager::CreateImage(const ImageDesc &desc)
	{
		auto handle = ImageHandle();
		auto &image = mImages[handle];
		VulkanUtils::CreateImage(desc.Width, desc.Height, desc.Depth, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image);

		return handle;
	}

	void GPUResourceManager::CreateImageView(ImageHandle h, const ImageViewDesc &desc)
	{
		if (!mImages.contains(h))
			return;

		auto &image = mImages[h];
		VulkanUtils::CreateImageView(image, desc.Type, desc.Format, desc.Aspect);
	}

	void GPUResourceManager::CreateSampler(ImageHandle h, const vk::SamplerCreateInfo &create_info)
	{
		if (!mImages.contains(h))
			return;

		auto &image = mImages[h];
		VulkanUtils::CreateImageSampler(image, create_info);
	}

	void GPUResourceManager::DestroyBuffer(BufferHandle h)
	{
		if (mBuffers.contains(h))
			mBuffers.erase(h);
	}

	void GPUResourceManager::DestroyImage(ImageHandle &h)
	{
		if (mImages.contains(h))
			mImages.erase(h);
	}

	Vulkan::AllocatedBuffer &GPUResourceManager::GetBuffer(BufferHandle &h)
	{
		static Vulkan::AllocatedBuffer empty;
		if (!mBuffers.contains(h))
			return empty;

		return mBuffers.at(h);
	}

	Vulkan::AllocatedImage &GPUResourceManager::GetImage(ImageHandle &h)
	{
		static Vulkan::AllocatedImage empty;
		if (!mImages.contains(h))
			return empty;

		return mImages.at(h);
	}

	GPUResourceManager &GPUResourceManager::Get()
	{
		static GPUResourceManager gpu_manager;
		return gpu_manager;
	}
} // namespace BHive