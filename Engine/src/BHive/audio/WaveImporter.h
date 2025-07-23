#pragma once

#include "core/Core.h"

namespace BHive
{
	class AudioSource;

	struct BHIVE_API WaveImporter
	{
		static Ref<AudioSource> Import(const std::filesystem::path &path);
	};

} // namespace BHive