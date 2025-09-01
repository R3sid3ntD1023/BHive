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
			CompileFromSource();
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
		GLenum binary_format = 0;
		std::vector<char> binary;

		glGetProgramiv(mProgramID, GL_PROGRAM_BINARY_LENGTH, &binary_length);

		binary.resize(binary_length);

		glGetProgramBinary(mProgramID, binary_length, nullptr, &binary_format, binary.data());

		ar(mFilePath, mSources, binary_format, binary);
	}

	void Shader::Load(cereal::BinaryInputArchive &ar)
	{

		GLenum binary_format = 0;
		std::vector<char> binary;

		ar(mFilePath, mSources, binary_format, binary);

		mProgramID = glCreateProgram();
		glProgramBinary(mProgramID, binary_format, binary.data(), binary.size());

		GLint status = 0;
		glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);

		if (!status)
		{
			GLchar infoLog[512];
			glGetProgramInfoLog(mProgramID, 512, nullptr, infoLog);
			LOG_ERROR("SHADER::PROGRAM BINARY PROGRAM LINKING : {} - {}", mName, infoLog);
			Compile();
		}
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

	void Shader::CompileFromSource()
	{
		std::string source;
		if (!FileSystem::ReadFile(mFilePath, source))
			return;

		PreProcess(source);
		Compile();
		Reflect();
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
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
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