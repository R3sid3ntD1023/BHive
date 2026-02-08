#include "core/threading/Threading.h"
#include "gfx/Texture.h"
#include "gfx/utils/texture/TextureUtils.h"
#include "TextureImporter.h"

#include <stb_image.h>
#include <stb_image_resize2.h>

namespace BHive
{
	namespace utils
	{
		EFormat GetFormatFromChannels(int32_t channels)
		{
			switch (channels)
			{
			case 1:
				return EFormat::R8;
			case 2:
				return EFormat::RG8;
			case 3:
				return EFormat::RGB8;
			case 4:
				return EFormat::RGBA8;
			default:
				break;
			}
			ASSERT(false);
			return EFormat::Invalid;
		}

		EFormat GetFormatFromChannelsHDR(int32_t channels)
		{
			switch (channels)
			{
			case 3:
				return EFormat::RGB32F;
			case 4:
				return EFormat::RGBA32F;
			default:
				break;
			}

			ASSERT(false);
			return EFormat::Invalid;
		}
	} // namespace utils

	bool TextureLoader::LoadImageData(const std::filesystem::path &file, int32_t &w, int32_t &h, int32_t &c, uint8_t *&data, int32_t flip)
	{
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load(flip);

		if (is_hdr)
		{
			data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c, 4);
		}
		else
		{
			data = stbi_load(path_str.c_str(), &w, &h, &c, 4);
		}

		if (!data)
		{
			LOG_ERROR("TextureImporter::Stbi - {}", stbi_failure_reason());
			return false;
		}

		return true;
	}

	Ref<Texture2D> TextureLoader::Import(const std::filesystem::path &file, const FTextureImportData &import_data)
	{
		int w = 0, h = 0, c_in = 0;
		const int forced_channels = 4;
		const int c_out = forced_channels;

		stbi_uc *image_data = nullptr;
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load((int)import_data.mFlip);

		size_t data_size = 0;
		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c_in, forced_channels);
			
			data_size = size_t(w ) * h * c_out * sizeof(float);
		}
		else
		{

			image_data = stbi_load(path_str.c_str(), &w, &h, &c_in, 4);
			data_size = size_t(w) * h * c_out;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Stbi : {} - {}", file, stbi_failure_reason());
			return nullptr;
		}

		FTextureCreateInfo create_info{};
		create_info.InternalFormat = is_hdr ? utils::GetFormatFromChannelsHDR(c_out) : utils::GetFormatFromChannels(c_out);
		create_info.Channels = c_out;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.MagFilter = EMagFilter::LINEAR;
		create_info.WrapMode = EWrapMode::REPEAT;
		create_info.GenerateMipMaps = 1;

		Ref<Texture2D> texture = nullptr;

		if (import_data.mWidth != 0 && import_data.mHeight != 0)
		{
			auto size = import_data.mWidth * import_data.mHeight * c_out;
			stbi_uc *resized_buffer = (stbi_uc *)malloc(size);
			stbir_resize_uint8_linear(image_data, w, h, 0, resized_buffer, import_data.mWidth, import_data.mHeight, 0, (stbir_pixel_layout)c_out);

			texture = Texture2D::Create((unsigned)import_data.mWidth, (unsigned)import_data.mHeight, create_info, resized_buffer, size);
		}
		else
		{
			texture = Texture2D::Create((unsigned)w, (unsigned)h, create_info, image_data, data_size);
		}

		stbi_image_free(image_data);

		return texture;
	}

	Ref<Texture2D> TextureLoader::LoadFromMemory(const uint8_t *data, int length)
	{
		int x = 0, y = 0, c = 0;
		stbi_uc *image_data = nullptr;

		stbi_set_flip_vertically_on_load(1);
		bool is_hdr = stbi_is_hdr_from_memory(data, length);
		size_t data_size = 0;

		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf_from_memory(data, length, &x, &y, &c, 4);
			data_size = x * y * c * sizeof(float);
		}
		else
		{
			image_data = stbi_load_from_memory(data, length, &x, &y, &c, 4);
			data_size = x * y * c;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Memeory - {}", stbi_failure_reason());
			return nullptr;
		}

		FTextureCreateInfo create_info{};
		create_info.InternalFormat = is_hdr ? utils::GetFormatFromChannelsHDR(c) : utils::GetFormatFromChannels(c);
		create_info.Channels = 4;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.MagFilter = EMagFilter::LINEAR;
		create_info.WrapMode = EWrapMode::REPEAT;
		create_info.GenerateMipMaps = 1;

		auto texture = Texture2D::Create((unsigned)x, (unsigned)y, create_info, image_data, data_size);

		stbi_image_free(image_data);

		return texture;
	}
} // namespace BHive