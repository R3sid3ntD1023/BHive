#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class AudioFactory : public Factory
	{
	public:
		virtual bool CanCreateNew() const { return false; }

		virtual Ref<Asset> Import(const std::filesystem::path &path) override;

		virtual std::string GetDefaultAssetName() const override { return "NewAudio"; }

		virtual std::vector<std::string> GetSupportedExtensions() { return {".ogg", ".wav"}; }

		virtual std::string GetDisplayName() const { return "Audio"; }

		REFLECTABLEV(Factory)
	};

} // namespace BHive
