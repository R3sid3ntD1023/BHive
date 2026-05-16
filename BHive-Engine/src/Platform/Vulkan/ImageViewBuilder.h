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

		static void Build(GPUImage &img, const ImageViewDesc &base, EViewTopology topo);

	private:
		static void BuildDefault(GPUImage &img, const ImageViewDesc &base);

		static void Build2DMips(GPUImage &img, const ImageViewDesc &base);

		static void Build2DArrayMips(GPUImage &img, const ImageViewDesc &base);

		static void BuildCube(GPUImage &img, const ImageViewDesc &base);

		static void BuildCubeArray(GPUImage &img, const ImageViewDesc &base);

		static void BuildFaceMips(GPUImage &img, const ImageViewDesc &base);

		static void Build3DMips(GPUImage &img, const ImageViewDesc &base);

	private:
		static ResourceID CreateFullView(GPUImage &img, const ImageViewDesc &base);

		static ResourceID CreateMipView(GPUImage &img, const ImageViewDesc &base, uint32_t mip);

		static ResourceID CreateCubeFullView(GPUImage &img, const ImageViewDesc &base);

		static ResourceID CreateCubeMipView(GPUImage &img, const ImageViewDesc &base, uint32_t mip);

		static ResourceID CreateFaceMipView(GPUImage &img, const ImageViewDesc &base, uint32_t face, uint32_t mip);
	};
}