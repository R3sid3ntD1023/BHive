#pragma once

#include "Platform/Vulkan/VulkanMemory.h"
#include "Platform/Vulkan/VulkanImageRegions.h"
#include "Platform/Vulkan/VulkanConverters.h"

namespace BHive
{
	struct ImageCreateInfo
	{
		vk::ImageCreateFlags CreateFlags{};
		uint32_t Width = 0, Height = 0, Depth = 1;
		vk::ImageType Type{};
		vk::ImageViewType ViewType{};
		FVulkanTextureCreateInfo CreateInfo{};
	};

	class VulkanImage
	{
	public:
		virtual ~VulkanImage();

		void Initialize(const ImageCreateInfo &createInfo);

		void Upload(const void *data, size_t size, const ImageCopyRegion &region = {}, const ImageSubresource &sub = {});

		NativeHandle GetNativeHandle() const { return Handle::Image(&mImage); }

		NativeHandle GetNativeHandle() { return Handle::Image(&mImage); }

	protected:
		virtual void OnInitialize(GPUImage &image, const ImageCreateInfo &createInfo) = 0;

	private:
		GPUImage mImage{};
	};

	class Image2D : public VulkanImage
	{
	public:
		virtual void OnInitialize(GPUImage& image, const ImageCreateInfo &createInfo) override;
	};

	class ImageCube : public VulkanImage
	{
	public:
		virtual void OnInitialize(GPUImage &image, const ImageCreateInfo &createInfo) override;
	};
}