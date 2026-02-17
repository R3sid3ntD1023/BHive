#pragma once

#include "core/Core.h"
#include "gfx/shader/ShaderAsset.h"
#include <shaderc/shaderc.hpp>

namespace BHive
{

	struct ShaderCompiler
	{
		ShaderCompiler(const std::filesystem::path &filepath);

		void Init();
		
		void Compile(ShaderAsset& asset);

		void CompileToVulkan(ShaderAsset& asset);

		void CompileToOpengl(ShaderAsset& asset);

	private:
		shaderc::Compiler mVulkanCompiler;
		shaderc::Compiler mOpenglCompiler;
		shaderc::CompileOptions mVulkanCompileOptions;
		shaderc::CompileOptions mOpenglCompileOptions;
		std::filesystem::path mFilePath;
	};
} // namespace BHive