#include "core/FileSystem.h"
#include "core/subsystem/SubSystem.h"
#include "Shader.h"
#include "utils/shader/ShaderCompiler.h"
#include "utils/shader/ShaderSerializer.h"
#include "utils/shader/ShaderTimeCache.h"
#include "utils/shader/ShaderUtils.h"
#include "core/Application.h"
#include "GraphicsContext.h"

namespace BHive
{
	namespace utils
	{
		vk::ShaderStageFlagBits GetAPIShaderStage(EShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage_Vertex:
				return vk::ShaderStageFlagBits::eVertex;
			case ShaderStage_Fragment:
				return vk::ShaderStageFlagBits::eFragment;
			case ShaderStage_Compute:
				return vk::ShaderStageFlagBits::eCompute;
			case ShaderStage_Geometry:
				return vk::ShaderStageFlagBits::eGeometry;
			default:
				break;
			}
			ASSERT(false)
			return vk::ShaderStageFlagBits::eAll;
		}
	} // namespace utils

	Shader::Shader(const std::filesystem::path &path)
		: mName(path.stem().string()),
		  mFilePath(path)
	{
		ShaderSerializer serializer;

		bool loaded_program_data = false;
		auto &shader_time_cache = GetSubSystem<ShaderTimeCache>();

		/*ShaderTimeCache::FileTime time{};
		bool was_modified = shader_time_cache.WasFileModified(path, &time);

		if (!was_modified)
		{
			loaded_program_data = serializer.Deserialize(path, *this);
		}
		else
		{
			LOG_TRACE("Shader::Shader() Shader was modified. Recompiling... {}", path);
		}

		if (!loaded_program_data)
		{
			CompileFromSource();
		}*/

		CompileFromSource();

		if (mProgramID)
		{
			mUniformSetter = CreateScope<ShaderUniformSetter>(mProgramID);
		}
	}

	Shader::~Shader()
	{
	}

	void Shader::Save(cereal::BinaryOutputArchive &ar) const
	{
		/*GLsizei binary_length = 0;
		GLenum binary_format = 0;
		std::vector<char> binary;

		glGetProgramiv(mProgramID, GL_PROGRAM_BINARY_LENGTH, &binary_length);

		binary.resize(binary_length);

		glGetProgramBinary(mProgramID, binary_length, nullptr, &binary_format, binary.data());

		ar(mFilePath, mSources, binary_format, binary);*/
	}

	void Shader::Load(cereal::BinaryInputArchive &ar)
	{

		/*GLenum binary_format = 0;
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
		}*/
	}

	const Shader::FShaderData &Shader::GetShaderData(EShaderStage stage) const
	{
		return mSources.at(stage);
	}

	void Shader::Compile()
	{
		auto &device = GraphicsContext::GetDevice();

		ShaderCompiler compiler;
		compiler.Init();

		for (auto &[stage, source] : mSources)
		{
			source.VulkanSpirv.clear();

			compiler.CompileToVulkan(mFilePath, stage, source.Code, source.VulkanSpirv);

			vk::ShaderModuleCreateInfo create_info{};
			create_info.codeSize = source.VulkanSpirv.size() * sizeof(uint32_t);
			create_info.pCode = source.VulkanSpirv.data();

			auto &shader_module = mVulkanShaderModules.emplace_back(device.createShaderModule(create_info));

			vk::PipelineShaderStageCreateInfo stage_info{};
			stage_info.stage = utils::GetAPIShaderStage(stage);
			stage_info.module = *shader_module;
			stage_info.pName = "main";
			mVulkanShaderStages.emplace_back(stage_info);
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
	}

	void Shader::UnBind() const
	{
	}

	void Shader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
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