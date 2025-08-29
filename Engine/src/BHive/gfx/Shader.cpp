#include "core/FileSystem.h"
#include "core/subsystem/SubSystem.h"
#include "Shader.h"
#include "utils/shader/ShaderCompiler.h"
#include "utils/shader/ShaderSerializer.h"
#include "utils/shader/ShaderTimeCache.h"
#include "utils/shader/ShaderUtils.h"
#include <glad/glad.h>

namespace BHive
{

	Shader::Shader(const std::filesystem::path &path)
		: mName(path.stem().string()),
		  mFilePath(path)
	{
		ShaderSerializer serializer;

		bool loaded_program_data = false;
		auto &shader_time_cache = GetSubSystem<ShaderTimeCache>();
		if (!shader_time_cache.WasFileModified(path))
		{
			loaded_program_data = serializer.Deserialize(path, *this);
		}

		if (!loaded_program_data)
		{
			std::string source;
			if (!FileSystem::ReadFile(path, source))
				return;

			PreProcess(source);
			Compile();
			Reflect();
		}

		if (mProgramID)
		{
			mUniformSetter = CreateScope<ShaderUniformSetter>(mProgramID);
		}
	}

	Shader::~Shader()
	{

		glDeleteProgram(mProgramID);
	}

	void Shader::Save(cereal::BinaryOutputArchive &ar) const
	{
		GLsizei binary_length = 0;
		glGetProgramiv(mProgramID, GL_PROGRAM_BINARY_LENGTH, &binary_length);

		GLenum binary_format = 0;
		void *binary = malloc(binary_length);

		glGetProgramBinary(mProgramID, binary_length, nullptr, &binary_format, binary);

		ar(mSources, binary_format, binary_length, cereal::binary_data(binary, binary_length));

		free(binary);
	}

	void Shader::Load(cereal::BinaryInputArchive &ar)
	{

		GLenum binary_format = 0;
		GLsizei binary_length = 0;

		ar(mSources, binary_format, binary_length);
		void *binary = malloc(binary_length);
		ar(cereal::binary_data(binary, binary_length));

		mProgramID = glCreateProgram();
		glProgramBinary(mProgramID, binary_format, binary, binary_length);

		glValidateProgram(mProgramID);

		GLint status = 0;
		glGetProgramiv(mProgramID, GL_VALIDATE_STATUS, &status);
		ASSERT(status, "Shader::Load() Failed to validate program");

		free(binary);
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

			auto program = glCreateProgram();
			glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(program, shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				LOG_ERROR("SHADER::COMPILE ERROR {} - {}, {}", mName, ShaderUtils::ToString(stage), infoLog);
				ASSERT(false);
			}

			glLinkProgram(program);

			glGetProgramiv(program, GL_LINK_STATUS, &status);
			if (!status)
			{
				glGetProgramInfoLog(program, 512, nullptr, infoLog);
				LOG_ERROR("SHADER::PROGRAM LINKING : {} - {}", mName, infoLog);
				ASSERT(false);
			}

			glDetachShader(program, shader);

			if (!status)
			{
				glDeleteProgram(program);
			}

			shaders.emplace_back(shader);
			mSeperablePrograms.emplace(stage, program);
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

		ShaderSerializer serializer;
		serializer.Serialize(mFilePath, *this);
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

	uint32_t Shader::GetSeperableProgram(EShaderStage stage) const
	{
		return mSeperablePrograms.at(stage);
	}

	void Shader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
		glDispatchCompute(w, h, d);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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

} // namespace BHive