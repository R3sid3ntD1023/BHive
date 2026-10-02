#include "Texture.h"
#include "RenderCommand.h"
#include "gfx/RenderCommand.h"
#include "gfx/RendererAPI.h"
#include "gfx/factories/TextureFactory.h"
#include "importers/TextureImporter.h"
#include <stb_image_resize2.h>

namespace BHive
{
	Texture2D::Texture2D(const glm::uvec2 &size, const FTextureCreateInfo &createInfo, const ByteBuffer &data)
		: mCreateInfo(createInfo),
		  mSize(size),
		  mBuffer(data)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->CreateTexture2D(mTextureID, size.x, size.y, createInfo);

		if (data)
		{
			SetData({
				.Data = data.GetData(),
				.Extent = {mSize.x, mSize.y, 1},
				.BaseArrayLayer = 0,
				.Layers = 1,
			});
		}
	}

	Texture2D::~Texture2D()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->DeleteTexture(mTextureID);
	}

	void Texture2D::SetData(const FTextureUploadInfo &info)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		auto size = mBuffer.GetSize();

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.BaseArrayLayer, .LayerCount = info.Layers, .Offset = info.Offset, .Extents = extents};
		ImageSubresourceRange range{info.BaseMipLevel, 1, info.BaseArrayLayer, info.Layers};
		api->SetTextureData(mTextureID, info.Data, size, region, range);
	}

	Texture2DArray::Texture2DArray(const glm::uvec2 &size, const FTextureCreateInfo &specification)
		: mSize(size),
		  mCreateInfo(specification)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->CreateTexture2DArray(mTextureID, size.x, size.y, specification);
	}

	Texture2DArray::~Texture2DArray()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->DeleteTexture(mTextureID);
	}

	void Texture2DArray::SetData(const FTextureUploadInfo &info)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		size_t size = mSize.x * mSize.y * GetBytesPerPixel(mCreateInfo.Format);

		glm::uvec3 extents = glm::compMul(info.Extent) == 0 ? glm::uvec3{mSize, 1} : info.Extent;
		ImageCopyRegion region{.BaseArrayLayer = info.BaseArrayLayer, .LayerCount = info.Layers, .Offset = info.Offset, .Extents = extents};
		ImageSubresourceRange range{info.BaseMipLevel, info.Levels, info.BaseArrayLayer, info.Layers};
		api->SetTextureData(mTextureID, info.Data, size, region, range);
	}

	int32_t Texture2DArray::Append(TexturePtr tex)
	{
		if (!tex)
		{
			return 0;
		}

		auto texture = tex.As<Texture2D>();
		if (texture->mLayerIndex != -1)
		{
			return texture->mLayerIndex;
		}

		const auto &tex_info = GetInfo();
		if (mCurrentLayer >= tex_info.ArrayLayers)
		{
			return -1;
		}

		if (uint32_t(mStoredTextures.size()) != tex_info.ArrayLayers)
			mStoredTextures.resize(tex_info.ArrayLayers);

		// upload resized texture
		const auto &buffer = texture->GetBuffer();
		const glm::ivec2 size = texture->GetSize();
		const glm::ivec2 output_size = GetSize();
		const auto bytes_per_pixel = GetBytesPerPixel(tex_info.Format);
		const auto buffer_size = output_size.x * output_size.y * bytes_per_pixel;
		ByteBuffer output(buffer_size);
		stbir_resize_uint8_linear(buffer.GetData(), size.x, size.y, 0, output, output_size.x, output_size.y, 0, (stbir_pixel_layout)bytes_per_pixel);

		FTextureUploadInfo info{
			.Data = output.GetData(),
			.Extent = {output_size.x, output_size.y, 1},
			.BaseArrayLayer = mCurrentLayer,
			.Layers = 1,
		};
		SetData(info);

		mStoredTextures[mCurrentLayer] = tex;

		texture->mLayerIndex = (int32_t)mCurrentLayer;

		return mCurrentLayer++;
	}

	void Texture2DArray::Clear()
	{
		mCurrentLayer = mStartLayer;
	}

	TexturePtr Texture2DArray::GetTexture(uint32_t index) const
	{
		return mStoredTextures[index];
	}

	TextureCube::TextureCube(uint32_t size, const FTextureCreateInfo &createInfo)
		: mCreateInfo(createInfo)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->CreateTextureCube(mTextureID, size, createInfo);
	}

	TextureCube::~TextureCube()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->DeleteTexture(mTextureID);
	}

	TextureCubeArray::TextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo)
		: mSize(size),
		  mCreateInfo(createInfo)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->CreateTextureCubeArray(mTextureID, size, createInfo);
	}

	TextureCubeArray::~TextureCubeArray()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->DeleteTexture(mTextureID);
	}

	Texture3D::Texture3D(const glm::uvec3 &size, const FTextureCreateInfo &createInfo, const ByteBuffer &data)
		: mCreateInfo(createInfo)
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->CreateTexture3D(mTextureID, size.x, size.y, size.z, createInfo);
		if (data)
		{
			SetData({
				.Data = data.GetData(),
				.Extent = {size.x, size.y, size.z},
				.BaseArrayLayer = 0,
				.Layers = 1,
			});
		}
	}

	Texture3D::~Texture3D()
	{
		auto api = RenderCommand::GetGraphicsAPI();
		api->DeleteTexture(mTextureID);
	}

} // namespace BHive