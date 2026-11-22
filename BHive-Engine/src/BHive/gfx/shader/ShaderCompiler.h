#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderAsset.h"

namespace BHive
{
	struct MacroDefinitions
	{
		std::unordered_map<EShaderStage, std::vector<std::string>> Names;

		bool contains(EShaderStage stage) const { return Names.contains(stage); }
	};

	struct ShaderCompiler
	{
		ShaderCompiler(const std::filesystem::path &filepath);

		void Init();

		void Compile(ShaderAsset &asset, const MacroDefinitions &definitions = {});

		void CompileToVulkan(ShaderAsset &asset, const MacroDefinitions &definitions = {});

		void CompileToOpengl(ShaderAsset &asset, const MacroDefinitions &definitions = {});

	private:
		std::filesystem::path mFilePath;
	};
} // namespace BHive