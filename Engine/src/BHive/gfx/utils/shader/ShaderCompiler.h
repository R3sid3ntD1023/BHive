#pragma once

#include "core/Core.h"
#include "gfx/ShaderStages.h"
#include <shaderc/shaderc.hpp>

namespace BHive
{
	struct ShaderCompiler
	{
		void Init();

		void CompileToVulkan(const std::filesystem::path &filename, EShaderStage stage, const std::string &src, std::vector<uint32_t> &binary);

		void CompileToOpengl(const std::filesystem::path &filename, EShaderStage stage, std::string &src, const std::vector<uint32_t> &spirv, std::vector<uint32_t> &opengl_spirv);

		static void WriteProgramBinary(const std::filesystem::path &filename, uint32_t program);

		static bool ReadProgramBinary(const std::filesystem::path &filename, uint32_t &program);

	private:
		shaderc::Compiler mVulkanCompiler;
		shaderc::Compiler mOpenglCompiler;
		shaderc::CompileOptions mVulkanCompileOptions;
		shaderc::CompileOptions mOpenglCompileOptions;
	};
} // namespace BHive