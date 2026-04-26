#include "VulkanImage.h"
#include "Platform/Vulkan/GPUResourceManager.h"
#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanBackend.h"

namespace BHive
{
	VulkanImage::~VulkanImage()
	{
		VulkanBackend::GetGPUResourceManager().DestroyImage(mImage);
	}

	void VulkanImage::Initialize(const ImageCreateInfo &createInfo)
	{
		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();

		ImageDesc desc{};
		desc.Flags = createInfo.CreateFlags;
		desc.Width = createInfo.Width;
		desc.Height = createInfo.Height;
		desc.Depth = createInfo.Depth;
		desc.ArrayLayers = createInfo.CreateInfo.ArrayLayers;
		desc.Format = createInfo.CreateInfo.Format;
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Tiling = vk::ImageTiling::eOptimal;
		desc.Usage = createInfo.CreateInfo.Usage;
		desc.Type = vk::ImageType::e2D;
		desc.BytesPerPixel = createInfo.CreateInfo.BytesPerPixel;
		desc.Aspect = createInfo.CreateInfo.Aspect;
		desc.MipLevels = createInfo.CreateInfo.MipLevels;
		desc.DebugName = createInfo.CreateInfo.DebugName;

		mImage = gpu_r_m.CreateImage(desc);

		OnInitialize(mImage, createInfo);

		vk::SamplerCreateInfo sampler_info(
			{}, createInfo.CreateInfo.MinFilter, createInfo.CreateInfo.MagFilter, vk::SamplerMipmapMode::eLinear, createInfo.CreateInfo.WrapMode, createInfo.CreateInfo.WrapMode,
			createInfo.CreateInfo.WrapMode, 0, 0, 1, createInfo.CreateInfo.CompareEnabled, createInfo.CreateInfo.CompareOp);
		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.f;
		sampler_info.minLod = 0.f;
		sampler_info.maxLod = 0.f;

		gpu_r_m.CreateSampler(mImage, sampler_info);

		VulkanBackend::SetObjectName(mImage.GetImage(), std::format("Image_{}", createInfo.CreateInfo.DebugName));
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

		auto& staging_buffer = VulkanBackend::GetGPUResourceManager().GetBuffer(stagingBuffer.Buffer);

		SingleTimeCommand cmd{};
		mImage.Transition(cmd, ImageState::TansferDst(), sub);
		VulkanUtils::CopyBufferToImage(cmd, staging_buffer, mImage.GetImage(), region);
		mImage.Transition(cmd, ImageState::ShaderRead(), sub);

		gpu_r_m.DestroyBuffer(stagingBuffer);
	}

	void Image2D::OnInitialize(GPUImage &image, const ImageCreateInfo &createInfo)
	{

		ImageViewDesc view_desc{};
		view_desc.Format = createInfo.CreateInfo.Format;
		view_desc.Type = createInfo.ViewType;
		view_desc.LayerCount = createInfo.CreateInfo.ArrayLayers;
		view_desc.Aspect = createInfo.CreateInfo.Aspect;

		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		gpu_r_m.Create2DViews(image, view_desc);
	}

	void ImageCube::OnInitialize(GPUImage &image, const ImageCreateInfo &createInfo)
	{
		ImageViewDesc view_desc{};
		view_desc.Format = createInfo.CreateInfo.Format;
		view_desc.Type = createInfo.ViewType;
		view_desc.LayerCount = createInfo.CreateInfo.ArrayLayers;
		view_desc.Aspect = createInfo.CreateInfo.Aspect;

		auto &gpu_r_m = VulkanBackend::GetGPUResourceManager();
		gpu_r_m.CreateCubeViews(image, view_desc);
	}
} // namespace BHive