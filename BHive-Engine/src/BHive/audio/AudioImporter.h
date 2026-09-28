#pragma once

#include "AudioSpecification.h"
#include "core/Core.h"

namespace BHive
{
	struct DecodedAudio
	{
		FAudioSpecification Specification;
		MemoryBlock<int16_t> Data;
	};

	struct BHIVE_API AudioImporter
	{
		static DecodedAudio Import(const std::filesystem::path &path);
	};

} // namespace BHive