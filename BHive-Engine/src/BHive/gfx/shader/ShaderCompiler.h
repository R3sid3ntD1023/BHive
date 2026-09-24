#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderAsset.h"
#include <shaderc/shaderc.hpp>

namespace BHive
{
	struct MacroDefinitions
	{
		std::unordered_map<EShaderStage, std::vector<const char *>> Names;

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
		shaderc::Compiler mVulkanCompiler;
		shaderc::Compiler mOpenglCompiler;
		shaderc::CompileOptions mVulkanCompileOptions;
		shaderc::CompileOptions mOpenglCompileOptions;
		std::filesystem::path mFilePath;
	};
} // namespace BHive