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
		std::filesystem::path SourcePath;
		std::unordered_map<EShaderStage, ShaderStageData> Stages;
		std::unordered_map<EShaderStage, FShaderReflection> Reflection;
	};

}