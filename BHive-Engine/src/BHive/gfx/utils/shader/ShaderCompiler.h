#pragma once

#include "core/Core.h"
#include "gfx/Shader.h"
#include <shaderc/shaderc.hpp>

namespace BHive
{

	struct ShaderCompiler
	{
		ShaderCompiler(const std::filesystem::path &filepath);

		void Init();

		void CompileToVulkan(EShaderStage stage, const std::string &src, std::vector<uint32_t> &spirv);

		void CompileToOpengl(EShaderStage stage, std::string &src, const std::vector<uint32_t> &spirv, std::vector<uint32_t> &opengl_spirv);

	private:
		shaderc::Compiler mVulkanCompiler;
		shaderc::Compiler mOpenglCompiler;
		shaderc::CompileOptions mVulkanCompileOptions;
		shaderc::CompileOptions mOpenglCompileOptions;
		std::filesystem::path mFilePath;
	};
} // namespace BHive