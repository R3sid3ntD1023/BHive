#include "core/threading/Threading.h"
#include "gfx/Framebuffer.h"
#include "gfx/Texture.h"
#include "ImageUtils.h"
#include <stb_image_write.h>
#include <stb_image_resize2.h>

namespace BHive
{
	void ImageUtils::SaveImage(const std::filesystem::path &path, unsigned w, unsigned h, unsigned c, void *data)
	{
		if (!std::filesystem::exists(path.parent_path()))
		{
			std::filesystem::create_directory(path.parent_path());
		}

		unsigned stride = c * w;
		stbi_flip_vertically_on_write(true);
		stbi_write_png(path.string().c_str(), w, h, c, data, stride);
	}

	void ImageUtils::SaveImage(const std::filesystem::path &path, Framebuffer *target, unsigned attachment)
	{
		const auto num_attachments = target->GetNumColorAttachments();
		ASSERT(attachment < num_attachments);

		Thread::Dispatch(
			[target, attachment, path]()
			{
				auto size = target->GetSpecification().Size;

				int32_t channels = 4;
				int32_t stride = channels * size.x;
				int32_t buffersize = stride * size.y;
				std::vector<uint8_t> buffer(buffersize);

				target->ReadPixel(attachment, 0, 0, size.x, size.y, buffer.data());
				ImageUtils::SaveImage(path, size.x, size.y, channels, buffer.data());
			});
	}

	void ImageUtils::SaveImage(const std::filesystem::path &path, Texture2D *texture)
	{
		auto &specs = texture->GetInfo();
		const auto c = GetBytesPerPixel(specs.Format);
		const auto size = texture->GetSize();
		const auto &data = texture->GetBuffer();

		if (!std::filesystem::exists(path.parent_path()))
		{
			std::filesystem::create_directory(path.parent_path());
		}

		unsigned stride = c * size.x * data.ValueSize;
		stbi_flip_vertically_on_write(true);
		stbi_write_png(path.string().c_str(), size.x, size.y, c, data, stride);
	}
} // namespace BHive
