#include "gfx/Texture.h"
#include "gfx/Enumerations.h"
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
			return EFormat::None;
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
			return EFormat::None;
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

	Ref<Texture2D> TextureLoader::CreateOrResizeTexture(const std::string& name, int32_t w, int32_t h, int32_t c, uint8_t *data, size_t size, bool hdr, const FTextureOverride &override)
	{
		FTextureCreateInfo create_info{};
		create_info.Format = hdr ? utils::GetFormatFromChannelsHDR(c) : utils::GetFormatFromChannels(c);
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.MagFilter = EMagFilter::LINEAR;
		create_info.WrapMode = EWrapMode::REPEAT;
		create_info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
		create_info.Aspect = ETextureAspect::Color;
		create_info.DebugName = name;

		if (override.Resize())
		{
			auto new_size = override.Width * override.Height * c * (hdr ? sizeof(float) : sizeof(char)) ;
			Buffer buffer(new_size);
			stbir_resize_uint8_linear(data, w, h, 0, buffer.As<uint8_t>(), override.Width, override.Height, 0, (stbir_pixel_layout)c);

			return Texture2D::Create({override.Width, override.Height}, create_info, buffer);
		}

		return Texture2D::Create({w, h}, create_info, Buffer(data, size));
	}

	Ref<Texture2D> TextureLoader::Import(const std::filesystem::path &file, const const FTextureOverride &override)
	{
		int w = 0, h = 0, c_in = 0;
		const int forced_channels = 4;
		const int c_out = forced_channels;

		stbi_uc *image_data = nullptr;
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load(1);

		size_t data_size = 0;
		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c_in, forced_channels);		
			data_size = size_t(w ) * h * c_out * sizeof(float);
		}
		else
		{

			image_data = stbi_load(path_str.c_str(), &w, &h, &c_in, forced_channels);
			data_size = size_t(w) * h * c_out;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Stbi : {} - {}", file, stbi_failure_reason());
			return nullptr;
		}


		Ref<Texture2D> texture = CreateOrResizeTexture(file.stem().string(),w, h, c_out, image_data, data_size, is_hdr, override);

		stbi_image_free(image_data);

		return texture;
	}

	Ref<Texture2D> TextureLoader::LoadFromMemory(const uint8_t *data, int length)
	{
		int w = 0, h = 0, c_in = 0;
		int c_out  = 4, forced_channels = 4;
		stbi_uc *image_data = nullptr;

		stbi_set_flip_vertically_on_load(1);
		bool is_hdr = stbi_is_hdr_from_memory(data, length);
		size_t data_size = 0;

		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf_from_memory(data, length, &w, &h, &c_in, forced_channels);
			data_size = w * h * c_out * sizeof(float);
		}
		else
		{
			image_data = stbi_load_from_memory(data, length, &w, &h, &c_in, forced_channels);
			data_size = w * h * c_out;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Memeory - {}", stbi_failure_reason());
			return nullptr;
		}

		auto texture = CreateOrResizeTexture("Memory Created", w, h, c_out, image_data, data_size, is_hdr, {});

		stbi_image_free(image_data);

		return texture;
	}
} // namespace BHive