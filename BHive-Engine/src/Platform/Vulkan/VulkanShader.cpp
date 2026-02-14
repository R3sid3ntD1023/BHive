#include "core/Application.h"
#include "core/FileSystem.h"
#include "core/subsystem/SubSystem.h"
#include "gfx/RenderCommand.h"
#include "gfx/utils/shader/ShaderCompiler.h"
#include "gfx/utils/shader/ShaderSerializer.h"
#include "gfx/utils/shader/ShaderTimeCache.h"
#include "gfx/utils/shader/ShaderUtils.h"
#include "VulkanShader.h"
#include "VulkanConverters.h"
#include "gfx/ShaderManager.h"

namespace BHive
{
	VulkanShader::VulkanShader(const std::filesystem::path &path)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mFilePath(path),
		  mName(path.stem().string())
	{
		ShaderSerializer serializer;

		bool loaded_program_data = false;
		auto &shader_time_cache = GetSubSystem<ShaderTimeCache>();

		ShaderTimeCache::FileTime time{};
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
		}

		CompileFromSource();
	}

	VulkanShader::VulkanShader(const std::string &name, const std::string &vert, const std::string &frag)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mFilePath(name),
		  mName(name)
	{
		mSources[EShaderStage::Vertex] = FShaderData(vert);
		mSources[EShaderStage::Fragment] = FShaderData(frag);

		Compile();
	}

	VulkanShader::~VulkanShader()
	{
		LOG_TRACE("VulkanShader Destructor Called")
	}

	void VulkanShader::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mFilePath, mSources);
	}

	void VulkanShader::Load(cereal::BinaryInputArchive &ar)
	{

		ar(mFilePath, mSources);
		Compile();
	}

	void VulkanShader::Reflect()
	{
		LOG_TRACE("Reflecting Shader... {}\n", GetName());

		for (auto &[stage, source] : mSources)
		{
			mRefl.Reflect(source.VulkanSpirv);

			LOG_TRACE("Stage: {}\n{}\n", ShaderUtils::ToString(stage), mRefl.to_string());
		}

	}

	void VulkanShader::Compile()
	{
		ShaderCompiler compiler(mFilePath);
		compiler.Init();

		for (auto &[stage, source] : mSources)
		{
			source.VulkanSpirv.clear();

			compiler.CompileToVulkan(stage, source.Code, source.VulkanSpirv);

			vk::ShaderModuleCreateInfo create_info({}, source.VulkanSpirv);
			mShaderModules.emplace(stage, mDevice.createShaderModule(create_info));
		}

		ShaderSerializer serializer;
		serializer.Serialize(mFilePath, *this);

		CreateDescriptorResources();
	}

	void VulkanShader::CompileFromSource()
	{
		std::string source;
		if (!FileSystem::ReadFile(mFilePath, source))
			return;

		PreProcess(source);
		Compile();
	}

	void VulkanShader::PreProcess(const std::string &source)
	{
		static std::string preprocessors =
			R"(
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

	void VulkanShader::Bind()
	{
		
	}

	void VulkanShader::UnBind()
	{
	}

	void VulkanShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
	}

	void VulkanShader::CreateDescriptorResources()
	{
		Reflect();

		const auto& refl = mRefl;

		auto num_samplers = (uint32_t)refl.Samplers.size();
		auto num_uniform_buffers = (uint32_t)refl.UniformBuffers.size();

		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (auto &[name, sampler] : refl.Samplers)
		{
			bindings.emplace_back(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);
		}

		for (auto &[name, uniform_buffer] : refl.UniformBuffers)
		{
			bindings.emplace_back(uniform_buffer.Binding, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
		}

		vk::DescriptorSetLayoutCreateInfo layout_info({}, bindings, nullptr);
		mDescriptorSetLayout = mDevice.createDescriptorSetLayout(layout_info);

		
	}

} // namespace BHive