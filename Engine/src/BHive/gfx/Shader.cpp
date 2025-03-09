#include "Shader.h"
#include <glad/glad.h>
#include "core/FileSystem.h"
#include "GraphicsContext.h"

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
		: mName(path.filename().string())
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

	void Shader::Compile()
	{

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
				LOG_ERROR("{} - {}, {}", mName, utils::GetTypeString(type), infoLog);
				ASSERT(false);
				return;
			}

			shaders.emplace_back(shader);
		}

		mShaderID = glCreateProgram();
		for (auto &shader : shaders)
		{
			glAttachShader(mShaderID, shader);
		}

		glLinkProgram(mShaderID);

		glGetProgramiv(mShaderID, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(mShaderID, 512, nullptr, infoLog);
			LOG_ERROR("{}", infoLog);
			ASSERT(false);
		}

		if (!status)
		{
			for (auto &shader : shaders)
			{
				glDeleteShader(shader);
			}

			glDeleteProgram(mShaderID);
		}

		for (auto &shader : shaders)
		{
			glDetachShader(mShaderID, shader);
		}

		LOG_INFO("ShaderID {} - {}", mShaderID, mName);

		Reflect();
	}

	void Shader::PreProcess(const std::string &source)
	{
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

		GLint uniform_count = 0, resource_count = 0, uniform_buffer_count = 0, storage_buffer_count = 0;

		glGetProgramInterfaceiv(mShaderID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);
		glGetProgramInterfaceiv(mShaderID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &resource_count);
		glGetProgramInterfaceiv(mShaderID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniform_buffer_count);
		glGetProgramInterfaceiv(mShaderID, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &storage_buffer_count);

		for (int i = 0; i < uniform_count; i++)
		{
			GLenum properties[] = {GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_LOCATION};
			GLint values[] = {0, 0, 0, 0};
			GLchar name[512];

			glGetProgramResourceiv(mShaderID, GL_UNIFORM, i, 4, properties, 4, nullptr, values);
			glGetProgramResourceName(mShaderID, GL_UNIFORM, i, 512, nullptr, name);

			FUniform uniform{.Type = values[0], .Size = values[1], .Offset = values[2], .Location = values[3]};
			mReflectionData.Uniforms.emplace(name, uniform);

			LOG_TRACE("\t{}:", name);
			LOG_TRACE("\t\t type-{}", values[0]);
			LOG_TRACE("\t\t size-{}", values[1]);
			LOG_TRACE("\t\t offset-{}", values[2]);
			LOG_TRACE("\t\t location-{}", values[3]);
		}

		LOG_TRACE("\tUniformBuffers");
		for (int i = 0; i < uniform_buffer_count; i++)
		{
			GLenum properties[2] = {GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
			GLint values[2] = {};
			GLchar name[512];

			glGetProgramResourceiv(mShaderID, GL_UNIFORM_BLOCK, i, 2, properties, 2, nullptr, values);
			glGetProgramResourceName(mShaderID, GL_UNIFORM_BLOCK, i, 512, nullptr, name);

			FUniformBufferData uniform{.Binding = values[0], .Size = values[1]};
			mReflectionData.UniformBuffers.emplace(name, uniform);

			LOG_TRACE("\t\t{} : binding-{}, size-{}", name, values[0], values[1]);
		}

		LOG_TRACE("\tStorageBuffers");

		for (int i = 0; i < storage_buffer_count; i++)
		{
			GLenum properties[2] = {GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
			GLint values[2] = {};
			GLint length;
			GLchar name[512];

			glGetProgramResourceiv(mShaderID, GL_SHADER_STORAGE_BLOCK, i, 2, properties, 2, nullptr, values);
			glGetProgramResourceName(mShaderID, GL_SHADER_STORAGE_BLOCK, i, 512, &length, name);

			mReflectionData.StorageBuffers.emplace(name, FStorageBuffer{.Binding = values[0], .Size = values[1]});
			LOG_TRACE("\t\t{} : binding-{}, size-{}", name, values[0], values[1]);
		}
	}

} // namespace BHive