#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/textures/VulkanTexture2D.h"
#include "Platform/Vulkan/textures/VulkanTexture2DArray.h"
#include "Platform/Vulkan/textures/VulkanTexture3D.h"
#include "Platform/Vulkan/textures/VulkanTextureCube.h"
#include "Platform/Vulkan/textures/VulkanTextureCubeArray.h"
#include "Texture.h"
#include <stb_image_resize2.h>

namespace BHive
{

	Ref<Texture2D> Texture2D::Create()
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2D>();
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const glm::uvec2& size, const FTextureCreateInfo &createInfo, const Buffer& data)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2D>(size, createInfo, data);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	int32_t Texture2DArray::AddTexture(const Ref<Texture2D> &tex)
	{
		if (!tex)
		{
			return 0;
		}

		if (tex->mLayerIndex != -1)
		{
			return tex->mLayerIndex;
		}

		const auto &tex_info = GetInfo();
		if (mLayerInfo.size() >= tex_info.ArrayLayers)
		{
			return -1;
		}

		const auto& buffer = tex->GetBuffer();
		const glm::ivec2 size = tex->GetSize();
		const glm::ivec2 output_size = GetSize();
		const auto format_layout = GetFormatLayout(tex_info.Format);
		const auto buffer_size = output_size.x * output_size.y * format_layout ;
		Buffer output( buffer_size);
		stbir_resize_uint8_linear(buffer.As<uint8_t>(), size.x, size.y, 0 , output, output_size.x, output_size.y, 0, (stbir_pixel_layout)format_layout);

		FTextureUploadInfo info{
			.Data = output.GetData(),
			.ArrayLayer = mCurrentLayer,
			.LayerCount = 1,
		};
		SetData(info);

		mLayerInfo.push_back({{(float)size.x / output_size.x, (float)size.y / output_size.y}, size});

		tex->mLayerIndex = (int32_t)mCurrentLayer;

		return mCurrentLayer++;
	}

	void Texture2DArray::Clear()
	{
		mCurrentLayer = mStartLayer;
		mLayerInfo.erase(mLayerInfo.begin() + mCurrentLayer, mLayerInfo.end());
	}

	const Texture2DArray::LayerInfo &Texture2DArray::GetLayerInfo(uint32_t layer) const
	{
		ASSERT(layer >= 0 && layer < mLayerInfo.size());
		return mLayerInfo.at(layer);
	}

	Ref<Texture2DArray> Texture2DArray::Create(const glm::uvec2& size, const FTextureCreateInfo &createInfo)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture2DArray>(size, createInfo);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<Texture3D> Texture3D::Create(const glm::uvec3& size, const FTextureCreateInfo &createInfo, const Buffer& data)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTexture3D>(size, createInfo, data);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(uint32_t size, const FTextureCreateInfo &createInfo)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTextureCube>(size, createInfo);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}

	Ref<TextureCubeArray> TextureCubeArray::Create(uint32_t size, const FTextureCreateInfo &createInfo)
	{
		switch (RenderCommand::GetGraphicsAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanTextureCubeArray>(size, createInfo);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}


	REFLECT(Texture)
	{
		BEGIN_REFLECT(Texture) REFLECT_PROPERTY_READ_ONLY("Size", GetSize);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture>>();
	}

	REFLECT(Texture2D)
	{
		BEGIN_REFLECT(Texture2D).constructor(rttr::select_overload<Ref<Texture2D>()>(&Texture2D::Create)) REFLECT_PROPERTY("Specification", GetInfo, SetInfo);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture2D>>();
	}
} // namespace BHive