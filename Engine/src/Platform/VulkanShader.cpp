#include "core/FileSystem.h"
#include "VulkanAPI.h"
#include "VulkanShader.h"
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
				return VK_SHADER_STAGE_VERTEX_BIT;
			if (type == "fragment")
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			if (type == "compute")
				return VK_SHADER_STAGE_COMPUTE_BIT;
			if (type == "geometry")
				return VK_SHADER_STAGE_GEOMETRY_BIT;

			ASSERT(false, "Invalid shader type ");
			return 0;
		}

		const char *GetTypeString(uint32_t type)
		{
			switch (type)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				return "VERTEX_SHADER";
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return "FRAGMENT_SHADER";
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return "COMPUTE_SHADER";
			case VK_SHADER_STAGE_GEOMETRY_BIT:
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
			case VK_SHADER_STAGE_VERTEX_BIT:
				return shaderc_glsl_vertex_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return shaderc_glsl_fragment_shader;
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return shaderc_glsl_compute_shader;
			case VK_SHADER_STAGE_GEOMETRY_BIT:
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

		const char *GetCacheVulkanFileExtension(uint32_t type)
		{
			switch (type)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				return ".cached_vulkan.vert";
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return ".cached_vulkan.frag";
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return ".cached_vulkan.comp";
			case VK_SHADER_STAGE_GEOMETRY_BIT:
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
			case VK_SHADER_STAGE_VERTEX_BIT:
				return ShaderType_Vertex;
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return ShaderType_Fragment;
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return ShaderType_Compute;
			case VK_SHADER_STAGE_GEOMETRY_BIT:
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

	VulkanShader::VulkanShader(const std::filesystem::path &path)
		: mName(path.stem().string()),
		  mFilePath(path)
	{
		std::string source;
		FileSystem::ReadFile(path, source);
		PreProcess(source);
		Compile();
	}

	VulkanShader::VulkanShader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader)
		: mName(name)
	{
		mSources[VK_SHADER_STAGE_VERTEX_BIT] = vertex_shader;
		mSources[VK_SHADER_STAGE_FRAGMENT_BIT] = fragment_shader;
		Compile();
	}

	VulkanShader::VulkanShader(const std::string &name, const std::string &compute_shader)
		: mName(name)
	{
		mSources[VK_SHADER_STAGE_COMPUTE_BIT] = compute_shader;
		Compile();
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::CreateCacheDirectoryIfNeeded()
	{
		auto cache_dir = utils::GetCacheDirectory();
		if (!std::filesystem::exists(cache_dir))
			std::filesystem::create_directories(cache_dir);
	}

	void VulkanShader::GetOrCreateVulkanBinaries(bool recompile)
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
			options.SetSourceLanguage(shaderc_source_language_glsl);
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

	void VulkanShader::Recompile()
	{
		if (mFilePath.empty())
			return;

		LOG_TRACE("Recompiling VulkanShader...");

		std::string source;
		FileSystem::ReadFile(mFilePath, source);
		PreProcess(source);
		Compile(true);
	}

	void VulkanShader::Compile(bool recompile)
	{
		CreateCacheDirectoryIfNeeded();
		GetOrCreateVulkanBinaries(recompile);

		auto device = VulkanAPI::GetAPI().GetDevice();
		std::unordered_map<uint32_t, VkShaderModule> modules;

		// create modules
		for (auto &[type, spirv] : mVulkanSpirv)
		{
			VkShaderModuleCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			create_info.codeSize = spirv.size() * sizeof(uint32_t);
			create_info.pCode = spirv.data();

			VK_ASSERT(
				vkCreateShaderModule(device, &create_info, nullptr, &modules[type]),
				"VulkanShader:: Failed to create module!");
		}

		for (auto &[type, shader_module] : modules)
		{
			VkPipelineShaderStageCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			create_info.stage = (VkShaderStageFlagBits)type;
			create_info.module = shader_module;
			create_info.pName = "main";
			mShaderStages.push_back(create_info);
		}

		for (auto &[type, module] : modules)
		{
			vkDestroyShaderModule(device, module, nullptr);
		}

		Reflect();
	}

	void VulkanShader::PreProcess(const std::string &source)
	{
		mSources.clear();

		auto token = "#type";
		auto version_token = "#version";

		auto token_length = strlen(token);
		auto version_length = strlen(version_token);

		std::string preprocessors =
			R"(
				#extension GL_EXT_scalar_block_layout: enable
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

	void VulkanShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
	}

	void VulkanShader::Reflect()
	{
		LOG_TRACE("Reflecting VulkanShader... {}\n", mName);

		for (auto &[type, source] : mVulkanSpirv)
		{
			auto type_name = utils::GetShaderTypeFromGL(type);
			auto reflection_data = mReflectionData[type_name];
			reflection_data.Reflect(source);
			LOG_TRACE("Stage - {} :\n {}\n", utils::GetTypeString(type), reflection_data.to_string());
		}
	}

} // namespace BHive