#include "ImageViewBuilder.h"
#include "VulkanBackend.h"

namespace BHive
{
	namespace utils
	{
		void ValidateViewRange(const GPUImage &img, const ImageViewDesc &d, const char *debugTag)
		{
			const uint32_t imageLayers = img.ArrayLayers;
			const uint32_t base = d.BaseArrayLayer;
			const uint32_t count = d.LayerCount;

			//basic sanity
			ASSERT(count > 0, "{}: layercount must be > 0", debugTag);
			ASSERT(base < imageLayers, "{}: baseArrayLayer {} >= imageLayers {}", debugTag, base, imageLayers);
			ASSERT(base + count <= imageLayers, "{}: baseArrayLayer {} + layerCount {} > imageLayers {}", debugTag, base, count, imageLayers);

			//cube-specfic rules
			if (d.Type == vk::ImageViewType::eCube || d.Type == vk::ImageViewType::eCubeArray)
			{
				ASSERT(count == 6, "{}: cube view must have layercount == 6 (got {})", debugTag, count);
				// For cube arrays, each cube is 6 layers
				ASSERT((base % 6) == 0, "{}: cube view baseArrayLayer {} must be multiple of 6", debugTag, base);
			}
		}
	}

	void ImageViewBuilder::Build(GPUImage &img, const ImageViewDesc &base, EViewTopology topo)
	{
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

	void ImageViewBuilder::BuildDefault(GPUImage &img, const ImageViewDesc &base)
	{
		img.Views.Default = CreateFullView(img, base);
	}

	void ImageViewBuilder::Build2DMips(GPUImage &img, const ImageViewDesc &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(img.MipLevels);

		for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::Build2DArrayMips(GPUImage &img, const ImageViewDesc &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(img.ArrayLayers);

		for (uint32_t layer = 0; layer < img.ArrayLayers; ++layer)
		{
			img.Views.Mips[layer].resize(img.MipLevels);
			for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
			{
				img.Views.Mips[layer][mip] = CreateMipView(img, base, mip);
			}
		}
	}

	void ImageViewBuilder::BuildCube(GPUImage &img, const ImageViewDesc &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		img.Views.CubeMips.resize(1);

		img.Views.CubeMips[0].resize(img.MipLevels);
		for (uint32_t mip = 0; mip < img.MipLevels; mip++)
		{
			img.Views.CubeMips[0][mip] = CreateCubeMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::BuildCubeArray(GPUImage &img, const ImageViewDesc &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		for (uint32_t cube = 0; cube < img.ArrayLayers; cube++)
		{
			img.Views.CubeMips[cube].resize(img.MipLevels);

			for (uint32_t mip = 0; mip < img.MipLevels; mip++)
			{
				ImageViewDesc d = base;
				d.Type = vk::ImageViewType::eCube;
				d.LevelCount = 1;

				d.BaseArrayLayer = cube * 6;
				d.LayerCount = 6;

				img.Views.CubeMips[cube][mip] = VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
			}
		}
	}

	void ImageViewBuilder::BuildFaceMips(GPUImage &img, const ImageViewDesc &base)
	{
		const uint32_t faces = 6;

		img.Views.Default = CreateCubeFullView(img, base);

		img.Views.Faces.resize(img.ArrayLayers);

		for (uint32_t layer = 0; layer < img.ArrayLayers; ++layer)
		{
			img.Views.Faces[layer].resize(faces);

			for (uint32_t f = 0; f < faces; ++f)
			{
				img.Views.Faces[layer][f].resize(img.MipLevels);

				for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
				{
					img.Views.Faces[layer][f][mip] = CreateFaceMipView(img, base, f, mip);
				}
			}
		}
	}

	void ImageViewBuilder::Build3DMips(GPUImage &img, const ImageViewDesc &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(img.MipLevels);

		for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	ResourceID ImageViewBuilder::CreateFullView(GPUImage &img, const ImageViewDesc &base)
	{
		ImageViewDesc d = base;
		d.BaseMipLevel = 0;
		d.LevelCount = img.MipLevels;
		d.BaseArrayLayer = 0;
		d.LayerCount = img.ArrayLayers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
	}

	ResourceID ImageViewBuilder::CreateMipView(GPUImage &img, const ImageViewDesc &base, uint32_t mip)
	{
		ImageViewDesc d = base;
		d.BaseMipLevel = mip;
		d.LevelCount = 1;
		d.BaseArrayLayer = 0;
		d.LayerCount = img.ArrayLayers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
	}

	ResourceID ImageViewBuilder::CreateCubeFullView(GPUImage &img, const ImageViewDesc &base)
	{
		ImageViewDesc d = base;
		d.Type = vk::ImageViewType::eCube;
		d.BaseMipLevel = 0;
		d.LevelCount = img.MipLevels;
		d.BaseArrayLayer = 0;
		d.LayerCount = 6;

		utils::ValidateViewRange(img, d, "CreateCubeFullView");

		return VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
	}

	ResourceID ImageViewBuilder::CreateCubeMipView(GPUImage &img, const ImageViewDesc &base, uint32_t mip)
	{
		ImageViewDesc d = base;
		d.Type = vk::ImageViewType::eCube;
		d.BaseMipLevel = mip;
		d.LevelCount = 1;
		d.BaseArrayLayer = 0;
		d.LayerCount = 6;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
	}

	ResourceID ImageViewBuilder::CreateFaceMipView(GPUImage &img, const ImageViewDesc &base, uint32_t face, uint32_t mip)
	{
		ImageViewDesc d = base;
		d.Type = vk::ImageViewType::e2D;
		d.BaseMipLevel = mip;
		d.LevelCount = 1;
		d.BaseArrayLayer = face;
		d.LayerCount = 1;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(img.GetImage(), d);
	}
} // namespace BHive