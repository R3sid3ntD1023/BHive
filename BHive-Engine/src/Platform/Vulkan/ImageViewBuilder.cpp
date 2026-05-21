#include "ImageViewBuilder.h"
#include "VulkanBackend.h"

namespace BHive
{
	namespace utils
	{
		void ValidateViewRange(const GPUImage &img, const vk::ImageViewCreateInfo &d, const char *debugTag)
		{
			const uint32_t imageLayers = img.ArrayLayers;
			const uint32_t base = d.subresourceRange.baseArrayLayer;
			const uint32_t count = d.subresourceRange.layerCount;

			//basic sanity
			ASSERT(count > 0, "{}: layercount must be > 0", debugTag);
			ASSERT(base < imageLayers, "{}: baseArrayLayer {} >= imageLayers {}", debugTag, base, imageLayers);
			ASSERT(base + count <= imageLayers, "{}: baseArrayLayer {} + layerCount {} > imageLayers {}", debugTag, base, count, imageLayers);

			//cube-specfic rules
			if (d.viewType == vk::ImageViewType::eCube || d.viewType == vk::ImageViewType::eCubeArray)
			{
				ASSERT(count == 6, "{}: cube view must have layercount == 6 (got {})", debugTag, count);
				// For cube arrays, each cube is 6 layers
				ASSERT((base % 6) == 0, "{}: cube view baseArrayLayer {} must be multiple of 6", debugTag, base);
			}
		}
	}

	void ImageViewBuilder::Build(GPUImage &img, const vk::ImageViewCreateInfo &base, EViewTopology topo)
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

	void ImageViewBuilder::BuildDefault(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		img.Views.Default = CreateFullView(img, base);
	}

	void ImageViewBuilder::Build2DMips(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(img.MipLevels);

		for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::Build2DArrayMips(GPUImage &img, const vk::ImageViewCreateInfo &base)
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

	void ImageViewBuilder::BuildCube(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		img.Views.CubeMips.resize(1);

		img.Views.CubeMips[0].resize(img.MipLevels);
		for (uint32_t mip = 0; mip < img.MipLevels; mip++)
		{
			img.Views.CubeMips[0][mip] = CreateCubeMipView(img, base, mip);
		}
	}

	void ImageViewBuilder::BuildCubeArray(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		img.Views.Default = CreateCubeFullView(img, base);

		for (uint32_t cube = 0; cube < img.ArrayLayers; cube++)
		{
			img.Views.CubeMips[cube].resize(img.MipLevels);

			for (uint32_t mip = 0; mip < img.MipLevels; mip++)
			{
				vk::ImageViewCreateInfo d = base;
				d.viewType = vk::ImageViewType::eCube;
				d.subresourceRange.levelCount = 1;
				d.subresourceRange.baseArrayLayer = cube * 6;
				d.subresourceRange.layerCount = 6;

				img.Views.CubeMips[cube][mip] = VulkanBackend::GetGPUResourceManager().CreateImageView(d);
			}
		}
	}

	void ImageViewBuilder::BuildFaceMips(GPUImage &img, const vk::ImageViewCreateInfo &base)
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

	void ImageViewBuilder::Build3DMips(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		BuildDefault(img, base);

		img.Views.Mips.resize(1);
		img.Views.Mips[0].resize(img.MipLevels);

		for (uint32_t mip = 0; mip < img.MipLevels; ++mip)
		{
			img.Views.Mips[0][mip] = CreateMipView(img, base, mip);
		}
	}

	ResourceID ImageViewBuilder::CreateFullView(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		vk::ImageViewCreateInfo d = base;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = img.MipLevels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = img.ArrayLayers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}", img.DebugName));
	}

	ResourceID ImageViewBuilder::CreateMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = img.ArrayLayers;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}]", img.DebugName, mip));
	}

	ResourceID ImageViewBuilder::CreateCubeFullView(GPUImage &img, const vk::ImageViewCreateInfo &base)
	{
		vk::ImageViewCreateInfo d = base;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = 0;
		d.subresourceRange.levelCount = img.MipLevels;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		utils::ValidateViewRange(img, d, "CreateCubeFullView");

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}", img.DebugName));
	}

	ResourceID ImageViewBuilder::CreateCubeMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base;
		d.viewType = vk::ImageViewType::eCube;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = 0;
		d.subresourceRange.layerCount = 6;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}]", img.DebugName, mip));
	}

	ResourceID ImageViewBuilder::CreateFaceMipView(GPUImage &img, const vk::ImageViewCreateInfo &base, uint32_t face, uint32_t mip)
	{
		vk::ImageViewCreateInfo d = base;
		d.viewType = vk::ImageViewType::e2D;
		d.subresourceRange.baseMipLevel = mip;
		d.subresourceRange.levelCount = 1;
		d.subresourceRange.baseArrayLayer = face;
		d.subresourceRange.layerCount = 1;

		return VulkanBackend::GetGPUResourceManager().CreateImageView(d, std::format("ImageView_{}[{}][{}]", img.DebugName, face, mip));
	}
} // namespace BHive