#include "TextureImporter.h"
#include "gfx/textures/Texture2D.h"
#include "gfx/TextureUtils.h"

#include <stb_image.h>
#include <stb_image_resize2.h>

namespace BHive
{
	Ref<Texture2D> TextureLoader::Import(const std::filesystem::path &file, const FTextureImportData &import_data)
	{
		int w = 0, h = 0, c = 0;
		stbi_uc *image_data = nullptr;
		auto path_str = file.string();
		bool is_hdr = stbi_is_hdr(path_str.c_str());
		stbi_set_flip_vertically_on_load((int)import_data.mFlip);

		size_t data_size = 0;
		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf(path_str.c_str(), &w, &h, &c, 0);
			data_size = w * h * c * sizeof(float);
		}
		else
		{

			image_data = stbi_load(path_str.c_str(), &w, &h, &c, 0);
			data_size = w * h * c;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Stbi - {}", stbi_failure_reason());
			return nullptr;
		}

		FTextureSpecification specification{};
		specification.InternalFormat = GetFormatFromChannels(is_hdr, c);
		specification.Channels = c;
		specification.MinFilter = EMinFilter::LINEAR;
		specification.MagFilter = EMagFilter::LINEAR;
		specification.WrapMode = EWrapMode::REPEAT;

		Ref<Texture2D> texture = nullptr;

		if (import_data.mWidth != 0 && import_data.mHeight != 0)
		{
			auto size = import_data.mWidth * import_data.mHeight * c;
			stbi_uc *resized_buffer = (stbi_uc *)malloc(size);
			stbir_resize_uint8_linear(
				image_data, w, h, 0, resized_buffer, import_data.mWidth, import_data.mHeight, 0, (stbir_pixel_layout)c);

			texture = CreateRef<Texture2D>(
				(unsigned)import_data.mWidth, (unsigned)import_data.mHeight, specification, resized_buffer, size);
		}
		else
		{
			texture = CreateRef<Texture2D>((unsigned)w, (unsigned)h, specification, image_data, data_size);
		}

		stbi_image_free(image_data);

		return texture;
	}

	Ref<Texture2D> TextureLoader::LoadFromMemory(const uint8_t *data, int length)
	{
		int x = 0, y = 0, comp = 0;
		stbi_uc *image_data = nullptr;

		stbi_set_flip_vertically_on_load(1);
		bool is_hdr = stbi_is_hdr_from_memory(data, length);
		size_t data_size = 0;

		if (is_hdr)
		{
			image_data = (stbi_uc *)stbi_loadf_from_memory(data, length, &x, &y, &comp, 0);
			data_size = x * y * comp * sizeof(float);
		}
		else
		{
			image_data = stbi_load_from_memory(data, length, &x, &y, &comp, 0);
			data_size = x * y * comp;
		}

		if (!image_data)
		{
			LOG_ERROR("TextureImporter::Memeory - {}", stbi_failure_reason());
			return nullptr;
		}

		FTextureSpecification specification{};
		specification.InternalFormat = GetFormatFromChannels(is_hdr, comp);
		specification.Channels = comp;
		specification.MinFilter = EMinFilter::LINEAR;
		specification.MagFilter = EMagFilter::LINEAR;
		specification.WrapMode = EWrapMode::REPEAT;

		auto texture = CreateRef<Texture2D>((unsigned)x, (unsigned)y, specification, image_data, data_size);

		stbi_image_free(image_data);

		return texture;
	}
} // namespace BHive