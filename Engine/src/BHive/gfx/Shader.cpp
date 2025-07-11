#include "core/FileSystem.h"
#include "GraphicsContext.h"
#include "Shader.h"
#include <glad/glad.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

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

		shaderc_shader_kind GetShadercType(uint32_t type)
		{
			switch (type)
			{
			case GL_VERTEX_SHADER:
				return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER:
				return shaderc_glsl_fragment_shader;
			case GL_COMPUTE_SHADER:
				return shaderc_glsl_compute_shader;
			case GL_GEOMETRY_SHADER:
				return shaderc_glsl_geometry_shader;
			default:
				break;
			}
			return shaderc_glsl_infer_from_source;
		}

		std::filesystem::path GetCacheDirectory()
		{
			return "cache/shaders/opengl";
		}

		const char *GetCacheOpenglFileExtension(uint32_t type)
		{
			switch (type)
			{
			case GL_VERTEX_SHADER:
				return ".cached_oepngl.vert";
			case GL_FRAGMENT_SHADER:
				return ".cached_opengl.frag";
			case GL_COMPUTE_SHADER:
				return ".cached_opengl.comp";
			case GL_GEOMETRY_SHADER:
				return ".cached_opengl.geom";

			default:
				break;
			}
			ASSERT(false)
			return "";
		}

		const char *GetCacheVulkanFileExtension(uint32_t type)
		{
			switch (type)
			{
			case GL_VERTEX_SHADER:
				return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:
				return ".cached_vulkan.frag";
			case GL_COMPUTE_SHADER:
				return ".cached_vulkan.comp";
			case GL_GEOMETRY_SHADER:
				return ".cached_vulkan.geom";
			default:
				break;
			}
			ASSERT(false)
			return "";
		}

		EShaderType GetShaderTypeFromGL(uint32_t type)
		{
			switch (type)
			{
			case GL_VERTEX_SHADER:
				return ShaderType_Vertex;
			case GL_FRAGMENT_SHADER:
				return ShaderType_Fragment;
			case GL_COMPUTE_SHADER:
				return ShaderType_Compute;
			case GL_GEOMETRY_SHADER:
				return ShaderType_Geometry;
			default:
				break;
			}
			ASSERT(false)
			return ShaderType_None;
		}

		struct IncludeHandler : public shaderc::CompileOptions::IncluderInterface
		{
			shaderc_include_result *GetInclude(
				const char *requested_source, shaderc_include_type type, const char *requesting_source,
				size_t include_depth) override
			{
				std::string content;

				std::filesystem::path directory = std::filesystem::path(requesting_source).parent_path();
				if (!std::filesystem::exists(directory))
				{
					directory = ENGINE_PATH "/data/shaders";
				}
				if (!FileSystem::ReadFile(directory / requested_source, content))
				{
					LOG_ERROR("Failed to read include file {}", requested_source);
					return nullptr;
				}

				auto result = new shaderc_include_result();
				result->source_name = requested_source;
				result->source_name_length = strlen(requested_source);
				result->content_length = content.size();
				result->content = new char[content.size() + 1];
				memcpy((char *)result->content, content.c_str(), content.size());
				return result;
			}
			void ReleaseInclude(shaderc_include_result *data) override { delete data; }
		};

	} // namespace utils

	Shader::Shader(const std::filesystem::path &path)
		: mName(path.stem().string()),
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
		: mName(name)
	{
		mSources[GL_COMPUTE_SHADER] = compute_shader;
		Compile();
	}

	Shader::~Shader()
	{

		glDeleteProgram(mShaderID);
	}

	void Shader::CreateCacheDirectoryIfNeeded()
	{
		auto cache_dir = utils::GetCacheDirectory();
		if (!std::filesystem::exists(cache_dir))
			std::filesystem::create_directories(cache_dir);
	}

	void Shader::GetOrCreateVulkanBinaries(bool recompile)
	{
		mVulkanSpirv.clear();
		for (const auto &[type, source] : mSources)
		{
			auto cache_path = utils::GetCacheDirectory() / (mName + utils::GetCacheVulkanFileExtension(type));
			if (std::filesystem::exists(cache_path) && !recompile)
			{
				FileSystem::ReadFile(cache_path, mVulkanSpirv[type]);
				continue;
			}

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			// options.SetOptimizationLevel(shaderc_optimization_level_performance);
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
			// options.SetSourceLanguage(shaderc_source_language_glsl);
			options.SetIncluder(std::make_unique<utils::IncludeHandler>());
			auto spirv_binary = compiler.CompileGlslToSpv(source, utils::GetShadercType(type), mName.c_str(), options);
			if (spirv_binary.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG_ERROR(
					"Failed to compile shader {}, stage {}-{}", mName, utils::GetTypeString(type),
					spirv_binary.GetErrorMessage());
				ASSERT(false);
			}
			else
			{
				mVulkanSpirv[type] = std::vector<uint32_t>(spirv_binary.cbegin(), spirv_binary.cend());
				FileSystem::WriteFile(cache_path, mVulkanSpirv[type]);
			}
		}
	}

	void Shader::GetOrCreateOpenGLBinaries(bool recompile)
	{
		mOpenglSpirv.clear();
		mOpenglSources.clear();
		for (auto &[type, source] : mSources)
		{
			auto cache_path = utils::GetCacheDirectory() / (mName + utils::GetCacheOpenglFileExtension(type));
			if (std::filesystem::exists(cache_path) && !recompile)
			{
				FileSystem::ReadFile(cache_path, mOpenglSpirv[type]);
				continue;
			}
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			// options.SetOptimizationLevel(shaderc_optimization_level_performance);
			options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			// options.SetSourceLanguage(shaderc_source_language_glsl);
			options.SetIncluder(std::make_unique<utils::IncludeHandler>());

			// spirv_cross::CompilerGLSL glsl_compiler(spirv);
			// mOpenglSources[type] = glsl_compiler.compile();
			// auto &source = mOpenglSources[type];

			auto spirv_binary = compiler.CompileGlslToSpv(source, utils::GetShadercType(type), mName.c_str(), options);
			if (spirv_binary.GetCompilationStatus() == shaderc_compilation_status_success)
			{
				mOpenglSpirv[type] = std::vector<uint32_t>(spirv_binary.cbegin(), spirv_binary.cend());
				FileSystem::WriteFile(cache_path, mOpenglSpirv[type]);
			}
			else
			{
				LOG_ERROR(
					"Failed to compile shader {} : stage:{}-{}", utils::GetTypeString(type), mName,
					spirv_binary.GetErrorMessage());
				ASSERT(false);
			}
		}
	}

	void Shader::Recompile()
	{
		if (mFilePath.empty())
			return;

		LOG_TRACE("Recompiling Shader...");

		std::string source;
		FileSystem::ReadFile(mFilePath, source);
		PreProcess(source);
		Compile(true);
	}

	void Shader::Compile(bool recompile)
	{
		CreateCacheDirectoryIfNeeded();
		GetOrCreateVulkanBinaries(recompile);
		GetOrCreateOpenGLBinaries(recompile);

		if (mShaderID != 0)
		{
			glDeleteProgram(mShaderID);
		}

		mShaderID = glCreateProgram();

		GLint status = 0;
		GLchar infoLog[512];
		std::vector<uint32_t> shaders;
		for (auto &[type, source] : mOpenglSpirv)
		{
			auto shader = glCreateShader(type);
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), source.size() * sizeof(uint32_t));
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(mShaderID, shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				LOG_ERROR("SHADER::COMPILE ERROR {} - {}, {}", mName, utils::GetTypeString(type), infoLog);
				ASSERT(false);
			}

			shaders.emplace_back(shader);
		}

		glLinkProgram(mShaderID);

		glGetProgramiv(mShaderID, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(mShaderID, 512, nullptr, infoLog);
			LOG_ERROR("SHADER::PROGRAM LINKING : {} - {}", mName, infoLog);
			ASSERT(false);
		}

		for (auto &shader : shaders)
		{
			glDetachShader(mShaderID, shader);
			glDeleteShader(shader);
		}

		if (!status)
		{
			glDeleteProgram(mShaderID);

			return;
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
				#extension GL_EXT_scalar_block_layout: enable
				#extension GL_ARB_gl_spirv : enable
				#extension GL_ARB_enhanced_layouts : enable
			)";

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
		LOG_TRACE("Reflecting Shader... {}\n", mName);

		for (auto &[type, source] : mVulkanSpirv)
		{
			mReflectionData.Reflect(source);
			LOG_TRACE("{}\n", mReflectionData.to_string());
		}
	}

} // namespace BHive