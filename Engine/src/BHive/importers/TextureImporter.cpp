#include "TextureImporter.h"
#include "gfx/Texture.h"
#include "gfx/TextureUtils.h"

#include <stb_image.h>
#include <stb_image_resize2.h>

namespace BHive
{
	Ref<Texture> TextureImporter::Import(const std::filesystem::path &file, const FTextureImportData &import_data)
	{
		int w = 0, h = 0, c = 0;
		stbi_uc *image_data = nullptr;
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load((int)import_data.mFlip);

		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c, 0);
		}
		else
		{

			image_data = stbi_load(path_str.c_str(), &w, &h, &c, 0);
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Stbi - {}", stbi_failure_reason());
			return nullptr;
		}

		FTextureSpecification specification{};
		specification.mFormat = GetFormatFromChannels(is_hdr, c);
		specification.mChannels = c;
		specification.mMinFilter = EFilterMode::LINEAR;
		specification.mMagFilter = EFilterMode::LINEAR;
		specification.mWrapMode = EWrapMode::REPEAT;

		Ref<Texture> texture = nullptr;
		unsigned char *resize_data = nullptr;

		if (import_data.mWidth != 0 && import_data.mHeight != 0)
		{
			auto size = import_data.mWidth * import_data.mHeight * c;
			resize_data = (unsigned char *)malloc(size);
			stbir_resize_uint8_linear(image_data, w, h, 0, resize_data, import_data.mWidth, import_data.mHeight, 0, (stbir_pixel_layout)c);

			texture = Texture2D::Create(resize_data, (unsigned)import_data.mWidth, (unsigned)import_data.mHeight, specification);
		}
		else
		{
			texture = Texture2D::Create(image_data, (unsigned)w, (unsigned)h, specification);
		}

		stbi_image_free(image_data);

		free(resize_data);

		return texture;
	}

	Ref<Texture> TextureImporter::LoadFromMemory(const uint8_t *data, int length)
	{
		int x = 0, y = 0, comp = 0;
		stbi_uc *image_data = nullptr;

		stbi_set_flip_vertically_on_load(1);
		bool is_hdr = stbi_is_hdr_from_memory(data, length);

		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf_from_memory(data, length, &x, &y, &comp, 0);
		}
		else
		{
			image_data = stbi_load_from_memory(data, length, &x, &y, &comp, 0);
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Memeory - {}", stbi_failure_reason());
			return nullptr;
		}

		FTextureSpecification specification{};
		specification.mFormat = GetFormatFromChannels(is_hdr, comp);
		specification.mChannels = comp;
		specification.mMinFilter = EFilterMode::LINEAR;
		specification.mMagFilter = EFilterMode::LINEAR;
		specification.mWrapMode = EWrapMode::REPEAT;

		auto texture = Texture2D::Create(image_data, (unsigned)x, (unsigned)y, specification);

		stbi_image_free(image_data);

		return texture;
	}
} // namespace BHive