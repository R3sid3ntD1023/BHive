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

	struct ImageViewBuildInfo
	{
		uint32_t Layers = 1;

		uint32_t Levels = 1;

		vk::ImageViewCreateInfo ViewCI{};

		std::string DebugName = "";
	};

	class ImageViewBuilder
	{
	public:

		static void Build(GPUImage &img, const ImageViewBuildInfo &base, EViewTopology topo);

	private:
		static void BuildDefault(GPUImage &img, const ImageViewBuildInfo &base);

		static void Build2DMips(GPUImage &img, const ImageViewBuildInfo &base);

		static void Build2DArrayMips(GPUImage &img, const ImageViewBuildInfo &base);

		static void BuildCube(GPUImage &img, const ImageViewBuildInfo &base);

		static void BuildCubeArray(GPUImage &img, const ImageViewBuildInfo &base);

		static void BuildFaceMips(GPUImage &img, const ImageViewBuildInfo &base);

		static void Build3DMips(GPUImage &img, const ImageViewBuildInfo &base);

	private:
		static ResourceID CreateFullView(GPUImage &img, const ImageViewBuildInfo &base);

		static ResourceID CreateMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t mip);

		static ResourceID CreateCubeFullView(GPUImage &img, const ImageViewBuildInfo &base);

		static ResourceID CreateCubeMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t mip);

		static ResourceID CreateFaceMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t face, uint32_t mip);
	};
}