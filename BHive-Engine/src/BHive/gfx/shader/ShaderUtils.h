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

		struct ExpandedSource
		{
			std::string Source;
			std::vector<std::filesystem::path> Dependencies;
		};

		static PreProcessData PreProcess(const std::string &source, const std::string &preprocessors = "");

		static std::string ExpandIncludes(const std::string &source, const std::string &requestingPath);

		static ExpandedSource ExpandIncludesWithDependencies(const std::string &source, const std::string &requestingPath);

		static std::string ResolveIncludePath(const std::string &requested, const std::string &requesting);

		static std::filesystem::path GetCacheDirectory();
	};
} // namespace BHive