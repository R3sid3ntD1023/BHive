#pragma once

#include "VulkanMemory.h"

namespace BHive
{
	enum class EViewTopology
	{
		DefaultOnly,
		Mips2D,
		Mips2DArray,
		Cube,
		CubeArray,
		FacesAndMips,
		Mips3D
	};

	class ImageViewBuilder
	{
	public:

		static void Build(GPUImage &img, const vk::ImageViewCreateInfo &base, EViewTopology topo);

	private:
		static void BuildDefault(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void Build2DMips(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void Build2DArrayMips(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void BuildCube(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void BuildCubeArray(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void BuildFaceMips(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static void Build3DMips(GPUImage &img, const vk::ImageViewCreateInfo &base);

	private:
		static ResourceID CreateFullView(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static ResourceID CreateMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t mip);

		static ResourceID CreateCubeFullView(GPUImage &img, const vk::ImageViewCreateInfo &base);

		static ResourceID CreateCubeMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t mip);

		static ResourceID CreateFaceMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t face, uint32_t mip);
	};
}