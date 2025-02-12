#include "core/FileSystem.h"
#include "GLShader.h"
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

	GLShader::GLShader(const std::filesystem::path &path)
		: mName(path.filename().string())
	{
		std::string source;
		FileSystem::ReadFile(path, source);
		PreProcess(source);
		Compile();
	}

	GLShader::GLShader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader)
		: mName(name)
	{
		mSources[GL_VERTEX_SHADER] = vertex_shader;
		mSources[GL_FRAGMENT_SHADER] = fragment_shader;
		Compile();
	}

	GLShader::~GLShader()
	{

		glDeleteProgram(mShaderID);
	}

	void GLShader::Compile()
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
			glDeleteProgram(mShaderID);
		}

		for (auto &shader : shaders)
		{
			glDeleteShader(shader);
		}

#if REFLECT_GL_SHADERS
		Reflect();
#endif
	}

	void GLShader::PreProcess(const std::string &source)
	{
		auto token = "#type";
		auto token_length = strlen(token);

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
			mSources.emplace(utils::GetTypeFromString(type), code);
		}
	}

	void GLShader::Bind() const
	{

		glUseProgram(mShaderID);
	}

	void GLShader::UnBind() const
	{

		glUseProgram(0);
	}

	void GLShader::SetUniform(int location, int value) const
	{
		glUniform1i(location, value);
	}

	void GLShader::SetUniform(int location, uint32_t value) const
	{
		glUniform1ui(location, value);
	}

	void GLShader::SetUniform(int location, float value) const
	{
		glUniform1f(location, value);
	}

	void GLShader::SetUniform(int location, const glm::vec2 &value) const
	{
		glUniform2fv(location, 1, &value.x);
	}

	void GLShader::SetUniform(int location, const glm::ivec2 &value) const
	{
		glUniform2iv(location, 1, &value.x);
	}

	void GLShader::SetUniform(int location, const glm::vec3 &value) const
	{
		glUniform3fv(location, 1, &value.x);
	}

	void GLShader::SetUniform(int location, const glm::vec4 &value) const
	{
		glUniform4fv(location, 1, &value.x);
	}

	void GLShader::SetUniform(int location, const glm::mat4 &value) const
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0].x);
	}

	void GLShader::SetBindlessTexture(int location, uint64_t texture) const
	{
		glProgramUniformHandleui64ARB(mShaderID, location, texture);
	}

	void GLShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{

		glDispatchCompute(w, h, d);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	int GLShader::GetUniformLocation(const std::string &name) const
	{
		return glGetUniformLocation(mShaderID, name.c_str());
	}

	void GLShader::Reflect()
	{
		LOG_TRACE("Reflecting GLShader... {}", mName);

		GLint uniform_count = 0, resource_count = 0, uniform_buffer_count = 0;

		glGetProgramInterfaceiv(mShaderID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);
		glGetProgramInterfaceiv(mShaderID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &resource_count);
		glGetProgramInterfaceiv(mShaderID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniform_buffer_count);

		for (int i = 0; i < uniform_count; i++)
		{
			GLenum properties[2] = {GL_ARRAY_SIZE, GL_TYPE};
			GLint values[2] = {};
			GLchar name[512];

			glGetProgramResourceiv(mShaderID, GL_UNIFORM, i, 2, properties, 2, nullptr, values);
			glGetProgramResourceName(mShaderID, GL_UNIFORM, i, 512, nullptr, name);

			Uniform uniform{.size = values[0], .type = values[1]};
			mReflectionData.uniforms.emplace(name, uniform);

			LOG_TRACE("\t{} : sz-{}, tp-{}", name, values[0], values[1]);
		}

		for (int i = 0; i < uniform_buffer_count; i++)
		{
			GLenum properties[2] = {GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
			GLint values[2] = {};
			GLchar name[512];

			glGetProgramResourceiv(mShaderID, GL_UNIFORM_BLOCK, i, 2, properties, 2, nullptr, values);
			glGetProgramResourceName(mShaderID, GL_UNIFORM_BLOCK, i, 512, nullptr, name);

			UniformBufferData uniform{.binding = values[0], .size = values[1]};
			mReflectionData.uniformBuffers.emplace(name, uniform);

			LOG_TRACE("\t{} : binding-{}, sz-{}", name, values[0], values[1]);
		}
	}

} // namespace BHive