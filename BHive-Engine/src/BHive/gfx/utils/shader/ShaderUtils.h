#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"

namespace BHive
{

	struct ShaderUtils
	{
		static std::string ToString(EShaderStage stage);

		static EShaderStage FromString(const std::string &str);

		using PreProcessData = std::unordered_map<EShaderStage, std::string>;

		static PreProcessData PreProcess(const std::string &source, const std::string &preprocessors = "");

		static std::filesystem::path GetCacheDirectory();
	};
} // namespace BHive