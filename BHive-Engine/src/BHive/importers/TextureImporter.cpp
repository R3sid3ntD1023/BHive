#include "gfx/Texture.h"
#include "gfx/Enumerations.h"
#include "TextureImporter.h"

#include <stb_image.h>
#include <stb_image_resize2.h>

namespace BHive
{
	namespace TextureUtils
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

		bool IsHDR(EFormat format)
		{
			return format == EFormat::RGB32F || format == EFormat::RGBA32F;
		}
	} // namespace TextureUtils

	bool TextureLoader::LoadImageData(const std::filesystem::path &file, int32_t &w, int32_t &h, int32_t &c, Buffer &buf, int32_t flip)
	{
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load(flip);

		stbi_uc *data = nullptr;
		size_t stride = sizeof(stbi_uc);

		if (is_hdr)
		{
			data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c, 4);
			stride = sizeof(float);
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

		buf.Allocate(data, w * h * c * stride);

		return true;
	}

	DecodedTexture TextureLoader::CreateDecodedTexture(const std::string &name, const glm::uvec3 &size, const Buffer &buf, bool hdr)
	{
		uint32_t c = size.z;

		FTextureCreateInfo info{};
		info.Format = hdr ? TextureUtils::GetFormatFromChannelsHDR(c) : TextureUtils::GetFormatFromChannels(c);
		info.MinFilter = EMinFilter::LINEAR;
		info.MagFilter = EMagFilter::LINEAR;
		info.WrapMode = EWrapMode::REPEAT;
		info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
		info.Aspect = ETextureAspect::Color;
		info.DebugName = name;

		DecodedTexture decoded;
		decoded.Size = size;
		decoded.CreateInfo = info;
		decoded.Data = buf;

		return decoded;
	}

	DecodedTexture TextureLoader::FromFile(const std::filesystem::path &file)
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
			data_size = size_t(w) * h * c_out * sizeof(float);
		}
		else
		{

			image_data = stbi_load(path_str.c_str(), &w, &h, &c_in, forced_channels);
			data_size = size_t(w) * h * c_out;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Stbi : {} - {}", file, stbi_failure_reason());
			return {};
		}

		Buffer data(image_data, data_size);

		stbi_image_free(image_data);

		return CreateDecodedTexture(file.stem().string(), {w, h, c_out}, data, is_hdr);
	}

	DecodedTexture TextureLoader::LoadFromMemory(const uint8_t *data, int length)
	{
		int w = 0, h = 0, c_in = 0;
		int c_out = 4, forced_channels = 4;
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
			return {};
		}

		Buffer outData(image_data, data_size);
		stbi_image_free(image_data);

		return CreateDecodedTexture("Memory Created", {w, h, c_out}, outData, is_hdr);
	}

	void TextureLoader::Resize(DecodedTexture &decodedTexture, const glm::uvec2 &requestedSize)
	{
		auto w = requestedSize.x;
		auto h = requestedSize.y;
		auto c = decodedTexture.Size.z;
		auto hdr = TextureUtils::IsHDR(decodedTexture.CreateInfo.Format);
		auto data = decodedTexture.Data;
		auto old_w = decodedTexture.Size.x;
		auto old_h = decodedTexture.Size.y;

		auto new_size = w * h * c * (hdr ? sizeof(float) : sizeof(char));
		Buffer newData(new_size);

		stbir_resize_uint8_linear(data.GetData(), old_w, old_h, 0, newData.GetData(), w, h, 0, (stbir_pixel_layout)c);

		decodedTexture.Data.Release();
		decodedTexture.Size = glm::uvec3(requestedSize, c);
		decodedTexture.Data = newData;
	}
} // namespace BHive