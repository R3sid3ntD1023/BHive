#include "core/FileSystem.h"
#include "GraphicsContext.h"
#include "Shader.h"
#include <glad/glad.h>

namespace BHive
{

	namespace utils
	{
		uint32_t GetTypeFromString(const std::string &type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment")
				return GL_FRAGMENT_SHADER;
			if (type == "compute")
				return GL_COMPUTE_SHADER;
			if (type == "geometry")
				return GL_GEOMETRY_SHADER;

			ASSERT(false, "Invalid shader type ");
			return 0;
		}

		const char *GetTypeString(uint32_t type)
		{
			switch (type)
			{
			case GL_VERTEX_SHADER:
				return "VERTEX_SHADER";
			case GL_FRAGMENT_SHADER:
				return "FRAGMENT_SHADER";
			case GL_COMPUTE_SHADER:
				return "COMPUTE_SHADER";
			case GL_GEOMETRY_SHADER:
				return "GEOMETRY_SHADER";
			default:
				break;
			}
			return "";
		}

		bool SetIncludeFromFile(const std::string &name, const char *filename)
		{
			std::string include_src;
			FileSystem::ReadFile(filename, include_src);

			if (include_src.empty())
				return false;

			const char *str = &include_src[0];
			glNamedStringARB(GL_SHADER_INCLUDE_ARB, (GLint)strlen(&name[0]), &name[0], (GLint)strlen(str), str);

			return true;
		}

		static const char *includePaths[] = {"/"};

	} // namespace utils

	Shader::Shader(const std::filesystem::path &path)
		: mName(path.filename().string()),
		  mFilePath(path)
	{
		std::string source;
		FileSystem::ReadFile(path, source);
		PreProcess(source);
		Compile();
	}

	Shader::Shader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader)
		: mName(name)
	{
		mSources[GL_VERTEX_SHADER] = vertex_shader;
		mSources[GL_FRAGMENT_SHADER] = fragment_shader;
		Compile();
	}

	Shader::Shader(const std::string &name, const std::string &compute_shader)
	{
		mSources[GL_COMPUTE_SHADER] = compute_shader;
		Compile();
	}

	Shader::~Shader()
	{

		glDeleteProgram(mShaderID);
	}

	void Shader::Recompile()
	{
		if (mFilePath.empty())
			return;

		LOG_TRACE("Recompiling Shader...");

		std::string source;
		FileSystem::ReadFile(mFilePath, source);
		PreProcess(source);
		Compile();
	}

	void Shader::Compile()
	{
		if (mShaderID != 0)
		{
			glDeleteProgram(mShaderID);
		}

		mShaderID = glCreateProgram();

		GLint status = 0;
		GLchar infoLog[512];
		std::vector<uint32_t> shaders;
		for (auto &[type, source] : mSources)
		{
			auto shader = glCreateShader(type);
			const char *code = source.data();
			glShaderSource(shader, 1, &code, nullptr);
			glCompileShaderIncludeARB(shader, 1, utils::includePaths, nullptr);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				LOG_ERROR("SHADER::COMPILE ERROR {} - {}, {}", mName, utils::GetTypeString(type), infoLog);
			}
			else
			{
				shaders.emplace_back(shader);
				glAttachShader(mShaderID, shader);
			}
		}

		glLinkProgram(mShaderID);

		glGetProgramiv(mShaderID, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(mShaderID, 512, nullptr, infoLog);
			LOG_ERROR("SHADER::PROGRAM LINKING : {}", infoLog);
		}

		if (!status)
		{
			for (auto &shader : shaders)
			{
				glDeleteShader(shader);
			}

			glDeleteProgram(mShaderID);

			return;
		}

		for (auto &shader : shaders)
		{
			glDetachShader(mShaderID, shader);
		}

		Reflect();
	}

	void Shader::PreProcess(const std::string &source)
	{
		mSources.clear();

		auto token = "#type";
		auto version_token = "#version";

		auto token_length = strlen(token);
		auto version_length = strlen(version_token);

		std::string preprocessors =
			R"(
				#extension GL_NV_uniform_buffer_std430_layout: enable
				#extension GL_ARB_shading_language_include : enable
				#extension GL_ARB_bindless_texture : enable
			)";
#ifdef USE_VERTEX_PULLING
		preprocessors += "\r\n#define USE_VERTEX_PULLING\r\n";
#endif // USE_VERTEX_PULLING

		auto pos = source.find(token, 0);
		while (pos != std::string::npos)
		{
			auto eol = source.find_first_of("\r\n", pos);
			ASSERT(eol != std::string::npos);
			auto begin = pos + token_length + 1;
			auto type = source.substr(begin, eol - begin);
			ASSERT(utils::GetTypeFromString(type), mName);

			auto next_pos = source.find_first_not_of("\r\n", eol);
			ASSERT(next_pos != std::string::npos);
			pos = source.find(token, next_pos);

			auto code = (pos == std::string::npos) ? source.substr(next_pos) : source.substr(next_pos, pos - next_pos);
			{
				auto v_start = code.find_first_of(version_token);
				auto v_end = code.find_first_of('\r\n', v_start);
				code.insert(code.begin() + v_end, preprocessors.begin(), preprocessors.end());
			}
			mSources.emplace(utils::GetTypeFromString(type), code);
		}
	}

	void Shader::Bind() const
	{

		glUseProgram(mShaderID);
	}

	void Shader::UnBind() const
	{

		glUseProgram(0);
	}

	void Shader::SetUniform(int location, int value) const
	{
		glProgramUniform1i(mShaderID, location, value);
	}

	void Shader::SetUniform(int location, uint32_t value) const
	{
		glProgramUniform1ui(mShaderID, location, value);
	}

	void Shader::SetUniform(int location, float value) const
	{
		glProgramUniform1f(mShaderID, location, value);
	}

	void Shader::SetUniform(int location, const glm::vec2 &value) const
	{
		glProgramUniform2fv(mShaderID, location, 1, &value.x);
	}

	void Shader::SetUniform(int location, const glm::ivec2 &value) const
	{
		glProgramUniform2iv(mShaderID, location, 1, &value.x);
	}

	void Shader::SetUniform(int location, const glm::vec3 &value) const
	{
		glProgramUniform3fv(mShaderID, location, 1, &value.x);
	}

	void Shader::SetUniform(int location, const glm::vec4 &value) const
	{
		glProgramUniform4fv(mShaderID, location, 1, &value.x);
	}

	void Shader::SetUniform(int location, const glm::mat4 &value) const
	{
		glProgramUniformMatrix4fv(mShaderID, location, 1, GL_FALSE, &value[0].x);
	}

	void Shader::SetUniform(int location, uint64_t texture) const
	{
		glProgramUniformHandleui64NV(mShaderID, location, texture);
	}

	void Shader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
		glDispatchCompute(w, h, d);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	int Shader::GetUniformLocation(const std::string &name) const
	{
		if (mUniformLocationCache.contains(name))
			return mUniformLocationCache.at(name);

		int location = glGetUniformLocation(mShaderID, name.c_str());
		mUniformLocationCache[name] = location;
		return location;
	}

	void Shader::Reflect()
	{
		LOG_TRACE("Reflecting Shader... {}", mName);

		mReflectionData = FShaderReflectionData(mShaderID);

		/*LOG_TRACE("\tUniforms");
		for (const auto &[name, uniform] : mReflectionData.Uniforms)
		{
			LOG_TRACE("\t\t name : {}", name);
			LOG_TRACE(
				"\t\t\t type : {}, size : {}, offset : {}, location : {}", uniform.Type, uniform.Size, uniform.Offset,
				uniform.Location);
		}*/

		LOG_TRACE("\tUniform Buffers");
		for (const auto &[name, buffer] : mReflectionData.UniformBuffers)
		{
			LOG_TRACE("\t\t name : {}", name);
			LOG_TRACE("\t\t\t binding : {}, size : {}", buffer.Binding, buffer.Size);

			/*for (auto &[member_name, member] : buffer.Uniforms)
			{
				LOG_TRACE("\t\t name : {}", member_name);
				LOG_TRACE(
					"\t\t\t type : {}, size : {}, offset : {}, location : {}", member.Type, member.Size, member.Offset,
					member.Location);
			}*/
		}

		LOG_TRACE("\tStorage Buffers");
		for (const auto &[name, buffer] : mReflectionData.StorageBuffers)
		{
			LOG_TRACE("\t\t name : {}", name);
			LOG_TRACE("\t\t\t binding : {}, size : {}", buffer.Binding, buffer.Size);
		}
	}

} // namespace BHive