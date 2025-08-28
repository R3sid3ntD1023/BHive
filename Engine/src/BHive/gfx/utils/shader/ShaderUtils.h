#pragma once

#include "core/Core.h"
#include "gfx/ShaderDataType.h"
#include "gfx/ShaderStages.h"

namespace BHive
{

	struct ShaderUtils
	{
		static uint32_t GetAPIShaderStage(EShaderStage stage);

		static uint32_t GetAPIShaderBit(EShaderStage stage);

		static std::string ToString(EShaderStage stage);

		static EShaderStage FromString(const std::string &str);

		using PreProcessData = std::unordered_map<EShaderStage, std::string>;

		static PreProcessData PreProcess(const std::string &source, const std::string &preprocessors = "");
	};
} // namespace BHive