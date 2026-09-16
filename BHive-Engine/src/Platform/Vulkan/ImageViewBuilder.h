#pragma once

#include "VulkanMemory.h"

namespace BHive
{

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
		static void Build(ImageViews &views, const ImageViewBuildInfo &base);

	private:
		static void BuildDefault(ImageViews &views, const ImageViewBuildInfo &base);

		static void Build2DMips(ImageViews &views, const ImageViewBuildInfo &base);

		static void Build2DArrayMips(ImageViews &views, const ImageViewBuildInfo &base);

		static void BuildCube(ImageViews &views, const ImageViewBuildInfo &base);

		static void BuildCubeArray(ImageViews &views, const ImageViewBuildInfo &base);

		static void BuildFaceMips(ImageViews &views, const ImageViewBuildInfo &base);

		static void Build3DMips(ImageViews &views, const ImageViewBuildInfo &base);

	private:
		static ResourceID CreateFullView(ImageViews &views, const ImageViewBuildInfo &base);

		static ResourceID CreateMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t mip);

		static ResourceID CreateCubeFullView(ImageViews &views, const ImageViewBuildInfo &base);

		static ResourceID CreateCubeMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t mip);

		static ResourceID CreateFaceMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t face, uint32_t mip);
	};
} // namespace BHive