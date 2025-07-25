#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class TextureFactory : public Factory
	{
	public:
		virtual Ref<Asset> Import(const std::filesystem::path &path) override;

		virtual Ref<Asset> Import(const uint8_t *data, size_t size);

		virtual std::vector<std::string> GetSupportedExtensions() { return {".png", ".jpg", ".jpeg", ".hdr"}; }

		virtual std::string GetDisplayName() const { return "Texture2D"; }

		REFLECTABLEV(Factory)
	};

	REFLECT_EXTERN(TextureFactory)
} // namespace BHive
