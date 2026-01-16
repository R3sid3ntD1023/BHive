#pragma once

#include "core/Core.h"
#include "gfx/Shader.h"
#include "gfx/ShaderDataType.h"

namespace BHive
{

	struct ShaderUtils
	{

		static std::string ToString(Shader::EShaderStage stage);

		static Shader::EShaderStage FromString(const std::string &str);

		using PreProcessData = std::unordered_map<Shader::EShaderStage, std::string>;

		static PreProcessData PreProcess(const std::string &source, const std::string &preprocessors = "");

		static std::filesystem::path GetCacheDirectory();
	};
} // namespace BHive