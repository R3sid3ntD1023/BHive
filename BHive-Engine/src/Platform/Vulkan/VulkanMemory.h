#pragma once

#include "gfx/NativeHandle.h"
#include "core/UUID.h"
#include "VulkanImageRegions.h"
#include "MemoryAllocator.h"

namespace BHive
{
	struct ImageSubresource;
	struct ImageViewDesc;
	class GPUResourceManager;

	struct ImageState
	{
		vk::ImageLayout Layout = vk::ImageLayout::eUndefined;

		vk::AccessFlags2 Access = {};

		vk::PipelineStageFlags2 Stage = {};

		static ImageState ShaderRead()
		{
			return {vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderSampledRead, vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader};
		}

		static ImageState ComputeWrite() { return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eShaderWrite, vk::PipelineStageFlagBits2::eComputeShader};}
	};

	struct Image
	{
		void SetImage(vk::Image image) { ImageSrc = image; }

		void SetAspect(vk::ImageAspectFlags aspect) { Aspect = aspect; }

		void CreateView(const ImageViewDesc &desc);

		const vk::ImageView& GetView() const;

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {});
	private:
		vk::Image ImageSrc = VK_NULL_HANDLE;

		UUID ViewHandle = NullID;

		vk::ImageAspectFlags Aspect;

		ImageState State = {vk::ImageLayout::eUndefined, {}, vk::PipelineStageFlagBits2::eTopOfPipe};

		friend GPUResourceManager;
	};

	struct AllocatedImage
	{
		const vk::Image &GetImage() const;

		const vk::ImageView &GetView() const;

		const vk::ImageView &GetMipView(uint32_t mip) const;

		const vk::Sampler &GetSampler() const;

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

		void GenerateMipViews(ImageViewDesc desc, uint32_t mipLevels);

		Image CreateImage();

	private:
		UUID ImageHandle = NullID;

		UUID ViewHandle = NullID;

		UUID SamplerHandle = NullID;

		std::vector<UUID> MipViews;

		std::vector<std::vector<ImageState>> MipStates;

		vk::ImageAspectFlags Aspect;

		uint32_t ArrayLayers = 1;

		MemoryAllocation Allocation;

		friend GPUResourceManager;
	};

	struct AllocatedBuffer
	{
		UUID Buffer = NullID;

		MemoryAllocation Allocation;

		vk::DeviceSize Size;

		const vk::Buffer& GetBuffer() const;

		~AllocatedBuffer();
	};

	struct Handle
	{
		static NativeHandle Image(const AllocatedImage *image) { return NativeHandle::FromPtr(image); }
		static NativeHandle Buffer(const AllocatedBuffer *buffer) { return NativeHandle::FromPtr(buffer); }
	};
} // namespace BHive