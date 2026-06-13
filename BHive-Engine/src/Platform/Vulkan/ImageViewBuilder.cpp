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
	}

	void ImageViewBuilder::Build(GPUImage &img, const ImageViewBuildInfo &base, EViewTopology topo)
	{
		//for debugging
		img.ViewType = base.ViewCI.viewType;

		switch (topo)
		{
		case BHive::EViewTopology::Mips2D:
			Build2DMips(img, base);
			break;
		case BHive::EViewTopology::Mips2DArray:
			Build2DArrayMips(img, base);
			break;
		case BHive::EViewTopology::Cube:
			BuildCube(img, base);
			break;
		case BHive::EViewTopology::CubeArray:
			BuildCubeArray(img, base);
			break;
		case BHive::EViewTopology::FacesAndMips:
			BuildFaceMips(img, base);
			break;
		case BHive::EViewTopology::Mips3D:
			Build3DMips(img, base);
			break;
		default:
			BuildDefault(img, base);
			break;
		}
	}

	void ImageViewBuilder::BuildDefault(GPUImage &img, const ImageViewBuildInfo &base)
	{
		img.Views.Default = CreateFullView(img, base);
	}

	void ImageViewBuilder::Build2DMips(GPUImage &img, const ImageViewBuildInfo &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(base.Levels);

		for (uint32_t mip = 0; mip < base.Levels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::Build2DArrayMips(GPUImage &img, const ImageViewBuildInfo &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(base.Layers);

		for (uint32_t layer = 0; layer < base.Layers; ++layer)
		{
			img.Views.Mips[layer].resize(base.Levels);
			for (uint32_t mip = 0; mip < base.Levels; ++mip)
			{
				img.Views.Mips[layer][mip] = CreateMipView(img, base, mip);
			}
		}
	}

	void ImageViewBuilder::BuildCube(GPUImage &img, const ImageViewBuildInfo &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		img.Views.CubeMips.resize(1);

		img.Views.CubeMips[0].resize(base.Levels);
		for (uint32_t mip = 0; mip < base.Levels; mip++)
		{
			img.Views.CubeMips[0][mip] = CreateCubeMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::BuildCubeArray(GPUImage &img, const ImageViewBuildInfo &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		for (uint32_t cube = 0; cube < base.Layers; cube++)
		{
			img.Views.CubeMips[cube].resize(base.Levels);

			for (uint32_t mip = 0; mip < base.Levels; mip++)
			{
				vk::ImageViewCreateInfo d = base.ViewCI;
				d.viewType = vk::ImageViewType::eCube;
				d.subresourceRange.levelCount = 1;
				d.subresourceRange.baseArrayLayer = cube * 6;
				d.subresourceRange.layerCount = 6;

				img.Views.CubeMips[cube][mip] = VulkanBackend::GetGPUResourceManager().CreateImageView(d);
			}
		}
	}

	void ImageViewBuilder::BuildFaceMips(GPUImage &img, const ImageViewBuildInfo &base)
	{
		const uint32_t faces = 6;

		img.Views.Default = CreateCubeFullView(img, base);

		img.Views.Faces.resize(base.Layers);

		for (uint32_t layer = 0; layer < base.Layers; ++layer)
		{
			img.Views.Faces[layer].resize(faces);

			for (uint32_t f = 0; f < faces; ++f)
			{
				img.Views.Faces[layer][f].resize(base.Levels);

				for (uint32_t mip = 0; mip < base.Levels; ++mip)
				{
					img.Views.Faces[layer][f][mip] = CreateFaceMipView(img, base, f, mip);
				}
			}
		}
	}

	void ImageViewBuilder::Build3DMips(GPUImage &img, const ImageViewBuildInfo &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(base.Levels);

		for (uint32_t mip = 0; mip < base.Levels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	ResourceID ImageViewBuilder::CreateFullView(GPUImage &img, const ImageViewBuildInfo &base)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = base.Levels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = base.Layers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}", img.DebugName));
	}

	ResourceID ImageViewBuilder::CreateMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = base.Layers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}]", img.DebugName, mip));
	}

	ResourceID ImageViewBuilder::CreateCubeFullView(GPUImage &img, const ImageViewBuildInfo &base)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = base.Levels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		utils::ValidateViewRange(base.Layers, base.Levels, d, "CreateCubeFullView");

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}", img.DebugName));
	}

	ResourceID ImageViewBuilder::CreateCubeMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}]", img.DebugName, mip));
	}

	ResourceID ImageViewBuilder::CreateFaceMipView(GPUImage &img, const ImageViewBuildInfo &base, uint32_t face, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base.ViewCI;
		d.viewType = vk::ImageViewType::e2D;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = face;
		d.subresourceRange.layerCount = 1;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}][{}]", img.DebugName, face, mip));
	}
} // namespace BHive