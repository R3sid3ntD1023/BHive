#pragma once

#include "ShaderReflection.h"

namespace BHive
{
	struct ShaderStageData
	{
		std::string Code;
		std::vector<uint32_t> Spirv;
	};

	struct ShaderAsset
	{
		std::string Name;
		std::string CacheName;
		std::string VariantName;
		std::filesystem::path SourcePath;
		std::vector<std::filesystem::path> Dependencies;
		std::unordered_map<EShaderStage, ShaderStageData> Stages;
		FShaderReflection MergedReflection;
	};

} // namespace BHive