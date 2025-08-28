#include "core/FileSystem.h"
#include "Shader.h"
#include "utils/shader/ShaderCompiler.h"
#include "utils/shader/ShaderUtils.h"
#include <glad/glad.h>

namespace BHive
{

	Shader::Shader(const std::filesystem::path &path)
		: mName(path.stem().string()),
		  mFilePath(path)
	{
		if (!ShaderCompiler::ReadProgramBinary(path, mProgramID))
		{
			std::string source;
			if (!FileSystem::ReadFile(path, source))
				return;

			PreProcess(source);
			Compile();
		}

		if (mProgramID)
		{
			mUniformSetter = CreateScope<ShaderUniformSetter>(mProgramID);

			Reflect();
		}
	}

	Shader::~Shader()
	{

		glDeleteProgram(mProgramID);
	}

	void Shader::Compile()
	{
		ShaderCompiler compiler;
		compiler.Init();

		for (auto &[stage, source] : mSources)
		{
			source.VulkanSpirv.clear();
			source.OpenglSpirv.clear();
			source.OpenglCompiledSource.clear();

			compiler.CompileToVulkan(mFilePath, stage, source.Code, source.VulkanSpirv);
			compiler.CompileToOpengl(mFilePath, stage, source.OpenglCompiledSource, source.VulkanSpirv, source.OpenglSpirv);
		}

		if (mProgramID != 0)
		{
			glDeleteProgram(mProgramID);
		}

		mProgramID = glCreateProgram();

		GLint status = 0;
		GLchar infoLog[512];
		std::vector<uint32_t> shaders;
		for (auto &[stage, source] : mSources)
		{
			auto shader_type = ShaderUtils::GetAPIShaderStage(stage);
			auto shader = glCreateShader(shader_type);
			const auto &binary = source.OpenglSpirv;

			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.data(), binary.size() * sizeof(uint32_t));
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(mProgramID, shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				LOG_ERROR("SHADER::COMPILE ERROR {} - {}, {}", mName, ShaderUtils::ToString(stage), infoLog);
				ASSERT(false);
			}

			shaders.emplace_back(shader);
		}

		glLinkProgram(mProgramID);

		glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(mProgramID, 512, nullptr, infoLog);
			LOG_ERROR("SHADER::PROGRAM LINKING : {} - {}", mName, infoLog);
			ASSERT(false);
		}

		for (auto &shader : shaders)
		{
			glDetachShader(mProgramID, shader);
			glDeleteShader(shader);
		}

		if (!status)
		{
			glDeleteProgram(mProgramID);

			return;
		}

		compiler.WriteProgramBinary(mFilePath, mProgramID);
	}

	void Shader::PreProcess(const std::string &source)
	{
		static std::string preprocessors =
			R"(
				#extension GL_EXT_scalar_block_layout: enable
				#extension GL_ARB_enhanced_layouts : enable
			)";

		mSources.clear();

		auto data = ShaderUtils::PreProcess(source, preprocessors);
		for (const auto &[stage, code] : data)
		{

			FShaderData data{};
			data.Code = code;
			mSources.emplace(stage, data);
		}
	}

	void Shader::Bind() const
	{

		glUseProgram(mProgramID);
	}

	void Shader::UnBind() const
	{

		glUseProgram(0);
	}

	void Shader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
		glDispatchCompute(w, h, d);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	ShaderUniformSetter *Shader::GetSetter() const
	{
		return mUniformSetter.get();
	}

	void Shader::Reflect()
	{
		LOG_TRACE("Reflecting Shader... {}\n", mName);

		for (auto &[stage, source] : mSources)
		{
			mReflectionData.Reflect(source.VulkanSpirv);

			LOG_TRACE("Stage: {}\n{}\n", ShaderUtils::ToString(stage), mReflectionData.to_string());
		}
	}

	PipelineShader::PipelineShader(const std::filesystem::path &path)
	{
		std::string source;
		if (!FileSystem::ReadFile(path, source))
			return;

		auto data = ShaderUtils::PreProcess(path.string());
		for (const auto &[stage, code] : data)
		{
			FShaderData data{};
			data.Code = code;
			mSources.emplace(stage, data);
		}

		ShaderCompiler compiler;
		compiler.Init();

		for (auto &[stage, source] : mSources)
		{
			source.VulkanSpirv.clear();
			source.OpenglSpirv.clear();
			source.OpenglCompiledSource.clear();

			compiler.CompileToVulkan(path, stage, source.Code, source.VulkanSpirv);
			compiler.CompileToOpengl(path, stage, source.OpenglCompiledSource, source.VulkanSpirv, source.OpenglSpirv);

			uint32_t shader = 0;
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, source.OpenglSpirv.data(), source.OpenglSpirv.size() * sizeof(uint32_t));
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
		}
	}

	void PipelineShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
		glDispatchCompute(w, h, d);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	ShaderUniformSetter *PipelineShader::GetSetter() const
	{
		return mUniformSetter.get();
	}

} // namespace BHive