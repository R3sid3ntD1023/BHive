#pragma once

#include "core/Core.h"

namespace BHive
{
	class AudioSource;

	struct WaveImporter
	{
		static BHIVE Ref<AudioSource> Import(const std::filesystem::path& path);
	};

}