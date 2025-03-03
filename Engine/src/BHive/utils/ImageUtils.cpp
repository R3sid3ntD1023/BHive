#include "ImageUtils.h"
#include "gfx/Framebuffer.h"
#include <stb_image_write.h>
#include <glad/glad.h>

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

	void ImageUtils::SaveImage(const std::filesystem::path &path, const Ref<Framebuffer> &target, unsigned attachment)
	{
		unsigned num_attachments = target->GetSpecification().Attachments.GetAttachments().size();
		ASSERT(attachment < num_attachments);

		try
		{
			auto w = target->GetSpecification().Width;
			auto h = target->GetSpecification().Height;

			int32_t channels = 4;
			int32_t stride = channels * w;
			int32_t buffersize = stride * h;
			std::vector<uint8_t> buffer(buffersize);

			target->ReadPixel(attachment, 0, 0, w, h, GL_UNSIGNED_BYTE, buffer.data());

			ImageUtils::SaveImage(path, w, h, channels, buffer.data());
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("Failed to save image");
		}
	}
} // namespace BHive
