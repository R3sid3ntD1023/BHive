#include "gfx/RenderCommand.h"
#include "Texture.h"
#include <stb_image_resize2.h>
#include "importers/TextureImporter.h"
#include "gfx/factories/TextureFactory.h"

namespace BHive
{

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
		Buffer output(buffer_size);
		stbir_resize_uint8_linear(buffer.As<uint8_t>(), size.x, size.y, 0, output, output_size.x, output_size.y, 0, (stbir_pixel_layout)bytes_per_pixel);

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

} // namespace BHive