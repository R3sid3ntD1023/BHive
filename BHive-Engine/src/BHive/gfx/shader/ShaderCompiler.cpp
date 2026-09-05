#include "ShaderCompiler.h"
#include "ShaderReflection.h"
#include "ShaderUtils.h"
#include "core/FileSystem.h"
#include "gfx/RenderCommand.h"
#include <regex>
#include <spirv_cross/spirv_glsl.hpp>

namespace BHive
{
	namespace utils
	{
		shaderc_shader_kind GetShadercType(EShaderStage stage)
		{
			switch (stage)
			{
			case EShaderStage::Vertex:
				return shaderc_glsl_vertex_shader;
			case EShaderStage::Fragment:
				return shaderc_glsl_fragment_shader;
			case EShaderStage::Compute:
				return shaderc_glsl_compute_shader;
			case EShaderStage::Geometry:
				return shaderc_glsl_geometry_shader;
			default:
				break;
			}
			return shaderc_glsl_infer_from_source;
		}

		struct IncludeHandler : public shaderc::CompileOptions::IncluderInterface
		{
			using UserDataType = std::pair<std::string, std::string>;

			shaderc_include_result *GetInclude(const char *requested_source, shaderc_include_type type, const char *requesting_source, size_t include_depth) override
			{
				auto resolved_path = ShaderUtils::ResolveIncludePath(requested_source, requesting_source);
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

		void ParseShaderArraySizes(const std::string &code, FShaderReflection &refl)
		{
			std::regex r(R"(layout\s*\(\s*set\s*=\s*(\d+)[^)]*\)\s*uniform\s+\w+\s+(\w+)\s* \[\s*(\d+)\s*\])");

			std::smatch match;

			std::string::const_iterator searchStart(code.begin());
			while (std::regex_search(searchStart, code.end(), match, r))
			{
				uint32_t set = std::stoi(match[1]);
				std::string name = match[2];
				uint32_t size = std::stoi(match[3]);

				auto &target_set = refl.Sets[set];
				if (target_set.Samplers.contains(name))
					target_set.Samplers[name].ArraySize = size;

				searchStart = match.suffix().first;
			}
		}

		struct SemanticTag
		{
			std::string VarName;
			std::string Semantic;
		};

		void ParseShaderSemantics(const std::string &code, std::unordered_map<std::string, std::string> &outVarToSemantic)
		{
			std::regex semanticRegex(R"(\/\/\s*@semantic\s+(\w+))");
			std::smatch match;

			std::string::const_iterator searchStart(code.begin());
			while (std::regex_search(searchStart, code.end(), match, semanticRegex))
			{
				std::string semantic = match[1];

				auto declStart = match.suffix().first;
				std::regex declRegex(R"((layout\s*\([^)]*\)\s*)?(uniform|buffer)\s+(\w+)\s*(\w+)?|\buniform\s+(\w+)\s*;|\bsampler\w*\s+(\w+)\s*;)");

				std::smatch declMatch;
				if (std::regex_search(declStart, code.end(), declMatch, declRegex))
				{
					std::string varName;

					// Cases:
					// layout(...) uniform Type varName { ... }
					// layout(...) buffer Type varName { ... }
					// uniform varName;
					// samplerCube varName;

					if (declMatch[4].matched) // layout(...) uniform Type varName
						varName = declMatch[4];
					else if (declMatch[3].matched) // layout(...) buffer Type varName
						varName = declMatch[3];
					else if (declMatch[5].matched) // uniform varName;
						varName = declMatch[5];
					else if (declMatch[6].matched) // samplerCube varName;
						varName = declMatch[6];

					if (!varName.empty())
						outVarToSemantic[varName] = semantic;
				}

				searchStart = match.suffix().first;
			}
		}

	} // namespace utils

	ShaderCompiler::ShaderCompiler(const std::filesystem::path &filepath)
		: mFilePath(filepath)
	{
	}

	void ShaderCompiler::Init()
	{
		mVulkanCompileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
		mVulkanCompileOptions.SetTargetSpirv(shaderc_spirv_version_1_6);

		mVulkanCompileOptions.SetIncluder(std::make_unique<utils::IncludeHandler>());

		mOpenglCompileOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		mOpenglCompileOptions.SetAutoMapLocations(true);
		mOpenglCompileOptions.SetIncluder(std::make_unique<utils::IncludeHandler>());
	}

	void ShaderCompiler::Compile(ShaderAsset &asset)
	{
		CompileToVulkan(asset);

		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			break;
		case RendererAPI::Opengl:
			CompileToOpengl(asset);
			break;
		default:
			break;
		}
	}

	void ShaderCompiler::CompileToVulkan(ShaderAsset &asset)
	{
		for (auto &[stage, data] : asset.Stages)
		{
			auto spirv_binary = mVulkanCompiler.CompileGlslToSpv(data.Code, utils::GetShadercType(stage), mFilePath.string().c_str(), mVulkanCompileOptions);
			if (spirv_binary.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG_ERROR("Vulkan: Failed to compile shader = {}, stage = {} : \n{}", mFilePath, ShaderUtils::ToString(stage), spirv_binary.GetErrorMessage());
				ASSERT(false);
			}

			data.Spirv.assign(spirv_binary.cbegin(), spirv_binary.cend());

			// reflect
			LOG_TRACE("Reflecting Shader... {}", asset.Name)
		}

		std::unordered_map<EShaderStage, FShaderReflection> refl_map;

		for (auto &[stage, data] : asset.Stages)
		{
			// parse semantics from glsl source
			std::unordered_map<std::string, std::string> varToSemantic;
			utils::ParseShaderSemantics(data.Code, varToSemantic);

			FShaderReflection refl{};

			refl.Reflect(stage, data.Spirv);

			refl.AttachSemantics(varToSemantic);

			utils::ParseShaderArraySizes(data.Code, refl);

			refl_map[stage] = refl;
		}

		LOG_TRACE("Merging Reflection");

		asset.MergedReflection = FShaderReflection::Merge(refl_map);

		LOG_TRACE(asset.MergedReflection.to_string())
	}

	void ShaderCompiler::CompileToOpengl(ShaderAsset &asset)
	{
		for (auto &[stage, data] : asset.Stages)
		{
			spirv_cross::CompilerGLSL glsl_compiler(data.Spirv);
			std::string glsl_source = glsl_compiler.compile();

			auto spirv_binary = mOpenglCompiler.CompileGlslToSpv(glsl_source, utils::GetShadercType(stage), mFilePath.string().c_str(), mOpenglCompileOptions);

			if (spirv_binary.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LOG_ERROR("GLSL: Failed to compile shader = {}, stage = {}: \n{}", mFilePath, ShaderUtils::ToString(stage), spirv_binary.GetErrorMessage());
				ASSERT(false);
			}

			data.Spirv.assign(spirv_binary.cbegin(), spirv_binary.cend());
			data.Code = std::move(glsl_source);
		}
	}

} // namespace BHive