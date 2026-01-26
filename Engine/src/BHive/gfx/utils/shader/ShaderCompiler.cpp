#include "core/FileSystem.h"
#include "ShaderCompiler.h"
#include "ShaderUtils.h"
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace BHive
{
	namespace utils
	{
		shaderc_shader_kind GetShadercType(Shader::EShaderStage stage)
		{
			switch (stage)
			{
			case Shader::ShaderStage_Vertex:
				return shaderc_glsl_vertex_shader;
			case Shader::ShaderStage_Fragment:
				return shaderc_glsl_fragment_shader;
			case Shader::ShaderStage_Compute:
				return shaderc_glsl_compute_shader;
			case Shader::ShaderStage_Geometry:
				return shaderc_glsl_geometry_shader;
			default:
				break;
			}
			return shaderc_glsl_infer_from_source;
		}

		const char *GetCacheOpenglFileExtension(Shader::EShaderStage stage)
		{
			switch (stage)
			{
			case Shader::ShaderStage_Vertex:
				return ".cached_opengl.vert";
			case Shader::ShaderStage_Fragment:
				return ".cached_opengl.frag";
			case Shader::ShaderStage_Compute:
				return ".cached_opengl.comp";
			case Shader::ShaderStage_Geometry:
				return ".cached_opengl.geom";

			default:
				break;
			}
			ASSERT(false)
			return "";
		}

		const char *GetCacheVulkanFileExtension(Shader::EShaderStage stage)
		{
			switch (stage)
			{
			case Shader::ShaderStage_Vertex:
				return ".cached_vulkan.vert";
			case Shader::ShaderStage_Fragment:
				return ".cached_vulkan.frag";
			case Shader::ShaderStage_Compute:
				return ".cached_vulkan.comp";
			case Shader::ShaderStage_Geometry:
				return ".cached_vulkan.geom";
			default:
				break;
			}
			ASSERT(false)
			return "";
		}

		struct IncludeHandler : public shaderc::CompileOptions::IncluderInterface
		{
			using UserDataType = std::pair<std::string, std::string>;

			shaderc_include_result *GetInclude(const char *requested_source, shaderc_include_type type, const char *requesting_source, size_t include_depth) override
			{
				auto resolved_path = ResolvePath(requested_source, requesting_source);
				std::string content;
				if (!FileSystem::ReadFile(resolved_path, content))
				{
					LOG_ERROR("ShaderIncluder::ERROR -Failed to read file : {}", requested_source);
					return nullptr;
				}

				return MakeIncludeResult(resolved_path, content);
			}

			void ReleaseInclude(shaderc_include_result *data) override
			{
				if (data)
				{
					delete (UserDataType *)data->user_data;
					delete data;
				}
			}

		private:
			std::string ResolvePath(const std::string &requested, const std::string &requesting)
			{
				std::filesystem::path directory = std::filesystem::path(requesting).parent_path();
				std::filesystem::path resolved_path = directory / requested;

				// use default engine path, if file isn't relative
				if (!std::filesystem::exists(resolved_path))
				{
					std::filesystem::recursive_directory_iterator it(ENGINE_SHADER_PATH);
					for (auto &entry : it)
					{
						auto file = entry.path().string();
						if (file.find(requested) != std::string::npos)
						{
							resolved_path = entry;
							break;
						}
					}
				}

				if (!std::filesystem::exists(resolved_path))
				{
					LOG_ERROR("ShaderIncluder::ERROR - Failed to find file : {} requsted from {}", requested, requesting);
					return "";
				}

				return resolved_path.string();
			}

			shaderc_include_result *MakeIncludeResult(const std::filesystem::path &resolved_path, const std::string &content)
			{
				auto *result = new shaderc_include_result();
				auto include_data = new UserDataType(resolved_path.string(), content);

				result->source_name = include_data->first.c_str();
				result->source_name_length = include_data->first.size();

				result->content = include_data->second.c_str();
				result->content_length = include_data->second.size();

				result->user_data = include_data;
				return result;
			}
		};

		void CreateCacheDirectory()
		{
			auto cache_dir = ShaderUtils::GetCacheDirectory();
			if (!std::filesystem::exists(cache_dir))
				std::filesystem::create_directories(cache_dir);
		}

	} // namespace utils

	ShaderCompiler::ShaderCompiler(const std::filesystem::path &filepath)
		: mFilePath(filepath)
	{
	}

	void ShaderCompiler::Init()
	{
		mVulkanCompileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		mVulkanCompileOptions.SetIncluder(std::make_unique<utils::IncludeHandler>());

		mOpenglCompileOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		mOpenglCompileOptions.SetAutoMapLocations(true);
		mOpenglCompileOptions.SetIncluder(std::make_unique<utils::IncludeHandler>());
	}

	void ShaderCompiler::CompileToVulkan(Shader::EShaderStage stage, const std::string &src, std::vector<uint32_t> &spirv)
	{

		auto name = mFilePath.stem().string();
		auto cache_path = ShaderUtils::GetCacheDirectory() / name / (name + utils::GetCacheVulkanFileExtension(stage));
		if (std::filesystem::exists(cache_path))
		{
			FileSystem::ReadFile(cache_path, spirv);
			return;
		}

		auto spirv_binary = mVulkanCompiler.CompileGlslToSpv(src, utils::GetShadercType(stage), mFilePath.string().c_str(), mVulkanCompileOptions);
		if (spirv_binary.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			LOG_ERROR("Vulkan: Failed to compile shader = {}, stage = {} : \n{}", mFilePath, ShaderUtils::ToString(stage), spirv_binary.GetErrorMessage());
			ASSERT(false);
		}
		else
		{
			spirv = std::vector<uint32_t>(spirv_binary.cbegin(), spirv_binary.cend());
			FileSystem::WriteFile(cache_path, spirv);
		}
	}

	void ShaderCompiler::CompileToOpengl(Shader::EShaderStage stage, std::string &src, const std::vector<uint32_t> &spirv, std::vector<uint32_t> &opengl_spirv)
	{

		auto name = mFilePath.stem().string();
		auto cache_path = ShaderUtils::GetCacheDirectory() / name / (name + utils::GetCacheOpenglFileExtension(stage));
		if (std::filesystem::exists(cache_path))
		{
			FileSystem::ReadFile(cache_path, opengl_spirv);
			return;
		}

		spirv_cross::CompilerGLSL glsl_compiler(spirv);
		src = glsl_compiler.compile();

		auto spirv_binary = mOpenglCompiler.CompileGlslToSpv(src, utils::GetShadercType(stage), mFilePath.string().c_str(), mOpenglCompileOptions);

		if (spirv_binary.GetCompilationStatus() == shaderc_compilation_status_success)
		{
			opengl_spirv = std::vector<uint32_t>(spirv_binary.cbegin(), spirv_binary.cend());
			FileSystem::WriteFile(cache_path, opengl_spirv);
		}
		else
		{
			LOG_ERROR("GLSL: Failed to compile shader = {}, stage = {}: \n{}", mFilePath, ShaderUtils::ToString(stage), spirv_binary.GetErrorMessage());
			ASSERT(false);
		}
	}

} // namespace BHive