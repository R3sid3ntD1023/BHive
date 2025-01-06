#include "ImageUtils.h"
#include <stb_image_write.h>

namespace BHive
{
	void ImageUtils::SaveImage(const std::filesystem::path& path, unsigned w, unsigned h, unsigned c, void* data)
	{
		if (!std::filesystem::exists(path.parent_path()))
		{
			std::filesystem::create_directory(path.parent_path());
		}

		unsigned stride = c * w;
		stbi_flip_vertically_on_write(true);
		stbi_write_png(path.string().c_str(), w, h, c, data, stride);
	}
}
