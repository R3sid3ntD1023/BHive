#include "ImageViewBuilder.h"
#include "VulkanBackend.h"

namespace BHive
{
	namespace utils
	{
		void ValidateViewRange(uint32_t layers, uint32_t levels, const vk::ImageViewCreateInfo &d, const char *debugTag)
		{
			const uint32_t imageLayers = layers;
			const uint32_t baseLayer = d.subresourceRange.baseArrayLayer;
			const uint32_t layerCount = d.subresourceRange.layerCount;

			const uint32_t baseMip = d.subresourceRange.baseMipLevel;
			const uint32_t mipCount = d.subresourceRange.levelCount;
			const uint32_t imageMips = levels;

			// --- Layer validation ---
			ASSERT(layerCount > 0, "{}: layerCount must be > 0", debugTag);
			ASSERT(baseLayer < imageLayers, "{}: baseArrayLayer {} >= imageLayers {}", debugTag, baseLayer, imageLayers);

			ASSERT(baseLayer + layerCount <= imageLayers, "{}: baseArrayLayer {} + layerCount {} > imageLayers {}", debugTag, baseLayer, layerCount, imageLayers);

			// --- Mip validation ---
			ASSERT(mipCount > 0, "{}: mipCount must be > 0", debugTag);
			ASSERT(baseMip < imageMips, "{}: baseMipLevel {} >= imageMipLevels {}", debugTag, baseMip, imageMips);

			ASSERT(baseMip + mipCount <= imageMips, "{}: baseMipLevel {} + mipCount {} > imageMipLevels {}", debugTag, baseMip, mipCount, imageMips);

			// --- Cube-specific rules ---
			if (d.viewType == vk::ImageViewType::eCube || d.viewType == vk::ImageViewType::eCubeArray)
			{
				ASSERT(layerCount == 6, "{}: cube view must have layerCount == 6 (got {})", debugTag, layerCount);

				ASSERT((baseLayer % 6) == 0, "{}: cube view baseArrayLayer {} must be multiple of 6", debugTag, baseLayer);
			}
		}
	} // namespace utils

	void ImageViewBuilder::Build(ImageViews &views, const ImageViewBuildInfo &base)
	{
		// for debugging
		auto viewType = base.ViewCI.viewType;

		switch (viewType)
		{
		case vk::ImageViewType::e2D:
			Build2DMips(views, base);
			break;
		case vk::ImageViewType::e2DArray:
			Build2DArrayMips(views, base);
			break;
		case vk::ImageViewType::eCube:
			BuildCube(views, base);
			break;
		case vk::ImageViewType::eCubeArray:
			BuildCubeArray(views, base);
			break;
		case vk::ImageViewType::e3D:
			Build3DMips(views, base);
			break;
		default:
			BuildDefault(views, base);
			break;
		}
	}

	void ImageViewBuilder::BuildDefault(ImageViews &views, const ImageViewBuildInfo &base)
	{
		views.Views[{0, 0}] = CreateFullView(views, base);
	}

	void ImageViewBuilder::Build2DMips(ImageViews &views, const ImageViewBuildInfo &base)
	{
		views.Views[{0, 0}] = CreateFullView(views, base);

		for (uint32_t mip = 0; mip < base.Levels; ++mip)
		{
			views.Views[{0, mip}] = CreateMipView(views, base, mip);
		}
	}

	void ImageViewBuilder::Build2DArrayMips(ImageViews &views, const ImageViewBuildInfo &base)
	{
		views.Views[{0, 0}] = CreateFullView(views, base);

		for (uint32_t layer = 0; layer < base.Layers; ++layer)
		{
			for (uint32_t mip = 0; mip < base.Levels; ++mip)
			{
				views.Views[{layer, mip}] = CreateMipView(views, base, mip);
			}
		}
	}

	void ImageViewBuilder::BuildCube(ImageViews &views, const ImageViewBuildInfo &base)
	{
		views.Views[{0, 0}] = CreateCubeFullView(views, base);

		for (uint32_t mip = 0; mip < base.Levels; mip++)
		{
			views.Views[{0, mip}] = CreateCubeMipView(views, base, mip);
		}
	}

	void ImageViewBuilder::BuildCubeArray(ImageViews &views, const ImageViewBuildInfo &base)
	{
		views.Views[{0, 0}] = CreateCubeFullView(views, base);
		const uint32_t cubeCount = base.Layers / 6;
		for (uint32_t cube = 0; cube < cubeCount; cube++)
		{
			for (uint32_t face = 0; face < 6; ++face)
			{
				uint32_t arrayLayer = cube * 6 + face;

				for (uint32_t mip = 0; mip < base.Levels; mip++)
				{
					vk::ImageViewCreateInfo d = base.ViewCI;
					d.viewType = vk::ImageViewType::e2D;

					d.subresourceRange.baseArrayLayer = cube * 6 + face;
					d.subresourceRange.layerCount = 1;
					d.subresourceRange.baseMipLevel = mip;
					d.subresourceRange.levelCount = 1;

					views.Views[{arrayLayer, mip}] = VulkanBackend::GetGPUResourceManager().CreateImageView(d);
				}
			}
		}
	}

	void ImageViewBuilder::BuildFaceMips(ImageViews &views, const ImageViewBuildInfo &base)
	{
		const uint32_t faces = 6;

		views.Views[{0, 0}] = CreateCubeFullView(views, base);

		for (uint32_t layer = 0; layer < base.Layers; ++layer)
		{
			for (uint32_t f = 0; f < faces; ++f)
			{
				for (uint32_t mip = 0; mip < base.Levels; ++mip)
				{
					views.Views[{layer, mip}] = CreateFaceMipView(views, base, f, mip);
				}
			}
		}
	}

	void ImageViewBuilder::Build3DMips(ImageViews &views, const ImageViewBuildInfo &base)
	{
		BuildDefault(views, base);

		for (uint32_t mip = 0; mip < base.Levels; ++mip)
		{
			views.Views[{0, mip}] = CreateMipView(views, base, mip);
		}
	}

	ResourceID ImageViewBuilder::CreateFullView(ImageViews &views, const ImageViewBuildInfo &base)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = base.Levels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = base.Layers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d);
	}

	ResourceID ImageViewBuilder::CreateMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = base.Layers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d);
	}

	ResourceID ImageViewBuilder::CreateCubeFullView(ImageViews &views, const ImageViewBuildInfo &base)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = base.Levels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		utils::ValidateViewRange(base.Layers, base.Levels, d, "CreateCubeFullView");

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d);
	}

	ResourceID ImageViewBuilder::CreateCubeMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d);
	}

	ResourceID ImageViewBuilder::CreateFaceMipView(ImageViews &views, const ImageViewBuildInfo &base, uint32_t face, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::e2D;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = face;
		d.subresourceRange.layerCount = 1;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d);
	}
} // namespace BHive