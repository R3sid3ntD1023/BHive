#include "VulkanMemory.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"

namespace BHive
{
	
	const vk::Image GPUImage::GetImage() const
	{

		return VulkanBackend::GetGPUResourceManager().GetImage(Image);
	}

	const vk::Sampler GPUImage::GetSampler() const
	{
		if (!Sampler.has_value())
			return nullptr;
		return VulkanBackend::GetGPUResourceManager().GetSampler(Sampler.value());
	}

	vk::ImageView GPUImage::GetView(uint32_t layer, uint32_t face, uint32_t mip) const
	{
		auto &rm = VulkanBackend::GetGPUResourceManager();

		if (!Views.Faces.empty())
		{
			return rm.GetImageView(Views.Faces[layer][face][mip]);
		}

		if (!Views.CubeMips.empty())
		{
			return rm.GetImageView(Views.CubeMips[layer][mip]);
		}

		if (!Views.Mips.empty())
		{
			return rm.GetImageView(Views.Mips[layer][mip]);
		}

		return rm.GetImageView(Views.Default);
	}

	const MemoryAllocation &AllocatedBuffer::GetAllocation() const
	{
		return VulkanBackend::GetGPUResourceManager().GetStorage<MemoryAllocation>().Get(Buffer);
	}

	const vk::Buffer &AllocatedBuffer::GetBuffer() const
	{
		return VulkanBackend::GetGPUResourceManager().GetBuffer(Buffer);
	}

	void ImageStateTracker::Initialize(uint32_t layers, uint32_t mips, const ImageState &initial)
	{
		ASSERT(layers > 0 && mips > 0, "layers and levels must be 1 or greater!")

		MipStates.resize(layers);
		for (uint32_t layer = 0; layer < layers; layer++)
		{
			MipStates[layer].assign(mips, initial);
		}
	}

	ImageState &ImageStateTracker::Get(uint32_t layer, uint32_t mip)
	{
		return MipStates[layer][mip];
	}
} 

