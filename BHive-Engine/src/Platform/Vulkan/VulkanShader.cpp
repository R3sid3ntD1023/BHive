#include "core/Application.h"
#include "core/FileSystem.h"
#include "core/subsystem/SubSystem.h"
#include "DescriptorBuilder.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "gfx/utils/shader/ShaderCompiler.h"
#include "gfx/utils/shader/ShaderSerializer.h"
#include "gfx/utils/shader/ShaderTimeCache.h"
#include "gfx/utils/shader/ShaderUtils.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "renderers/buffers/GlobalBuffers.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanShader.h"
#include "VulkanUtils.h"

namespace BHive
{
	namespace utils
	{
		vk::ShaderStageFlagBits GetAPIShaderStage(Shader::EShaderStage stage)
		{
			switch (stage)
			{
			case Shader::ShaderStage_Vertex:
				return vk::ShaderStageFlagBits::eVertex;
			case Shader::ShaderStage_Fragment:
				return vk::ShaderStageFlagBits::eFragment;
			case Shader::ShaderStage_Compute:
				return vk::ShaderStageFlagBits::eCompute;
			case Shader::ShaderStage_Geometry:
				return vk::ShaderStageFlagBits::eGeometry;
			default:
				break;
			}
			ASSERT(false)
			return vk::ShaderStageFlagBits::eAll;
		}

		vk::PrimitiveTopology GetTopology(EDrawMode mode)
		{
			switch (mode)
			{
			case BHive::Lines:
				return vk::PrimitiveTopology::eLineList;
			default:
				break;
			}

			return vk::PrimitiveTopology::eTriangleList;
		}

		vk::CullModeFlagBits GetCullMode(ECullMode mode)
		{
			switch (mode)
			{
			case ECullMode::Cull_Back:
				return vk::CullModeFlagBits::eBack;
			case ECullMode::Cull_Front:
				return vk::CullModeFlagBits::eFront;
			case ECullMode::Cull_None:
				return vk::CullModeFlagBits::eNone;
			default:
				break;
			}
			return vk::CullModeFlagBits::eBack;
		}
	} // namespace utils

	VulkanShader::VulkanShader(const std::filesystem::path &path, const FRenderOptions &options)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mFilePath(path),
		  mName(path.stem().string()),
		  mRenderOptions(options)
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
	}

	VulkanShader::VulkanShader(const std::string &name, const std::string &vert, const std::string &frag, const FRenderOptions &options)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mFilePath(name),
		  mName(name),
		  mRenderOptions(options)
	{
		mSources[Shader::ShaderStage_Vertex] = FShaderData(vert);
		mSources[Shader::ShaderStage_Fragment] = FShaderData(frag);

		Compile();
	}

	VulkanShader::~VulkanShader()
	{
		
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

		Reflect();
		CreateDescriptorResources();
		CreatePipeline();
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
		mGraphicsPipeline->Bind();
	}

	void VulkanShader::UnBind()
	{
	}

	void VulkanShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
	}

	void VulkanShader::CreateDescriptorResources()
	{
		auto num_samplers = (uint32_t)mReflectionData.Samplers.size();
		auto num_uniform_buffers = (uint32_t)mReflectionData.UniformBuffers.size();

		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (auto &[name, sampler] : mReflectionData.Samplers)
		{
			bindings.emplace_back(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);
		}

		for (auto &[name, uniform_buffer] : mReflectionData.UniformBuffers)
		{
			bindings.emplace_back(uniform_buffer.Binding, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
		}

		vk::DescriptorSetLayoutCreateInfo layout_info({}, bindings, nullptr);
		mDescriptorSetLayout = mDevice.createDescriptorSetLayout(layout_info);
	}

	void VulkanShader::Reflect()
	{
		LOG_TRACE("Reflecting Shader... {}\n", GetName());

		for (auto &[stage, source] : mSources)
		{
			mReflectionData.Reflect(source.VulkanSpirv);

			LOG_TRACE("Stage: {}\n{}\n", ShaderUtils::ToString(stage), mReflectionData.to_string());
		}
	}

	void VulkanShader::CreatePipeline()
	{
		auto swap_chain = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get()).GetSwapChain();

		std::vector<vk::PipelineShaderStageCreateInfo> create_infos;

		for (auto &[stage, module] : mShaderModules)
		{
			create_infos.emplace_back(vk::PipelineShaderStageCreateFlags{}, utils::GetAPIShaderStage(stage), module, "main");
		}

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, *mDescriptorSetLayout);
		mPipelineLayout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		vk::PipelineRenderingCreateInfo rendering_info{};
		rendering_info.setViewMask(0).setColorAttachmentCount(1).setColorAttachmentFormats(swap_chain->GetFormat().format);

		FVulkanPipelineConfigInfo config = VulkanPipeline::GetDefaultConfigInfo();
		config.Layout = mPipelineLayout;
		config.Next = &rendering_info;
		config.ShaderCreateInfos = create_infos;
		config.InputAssembly.setTopology(utils::GetTopology(mRenderOptions.DrawMode));
		config.Rasterazation.setCullMode(utils::GetCullMode(mRenderOptions.CullMode));
	//	config.DepthStencil.setDepthTestEnable(VK_FALSE).setDepthWriteEnable(VK_FALSE);

		mGraphicsPipeline = Pipeline::Create();
		mGraphicsPipeline->Init(config);
	}

	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		
	}

	void VulkanBackendMaterial::Init(const Ref<Shader> &shader)
	{	
		auto vulkan_shader = Cast<VulkanShader>(shader);
		
		ASSERT(vulkan_shader)

		mShader = vulkan_shader;

		const auto &shader_reflection = vulkan_shader->GetRelectionData();

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		std::vector<vk::DescriptorSetLayout> layouts(VulkanCore::MAX_FRAMES_IN_FLIGHT, vulkan_shader->GetDescriptorSetLayout());
		vk::DescriptorSetAllocateInfo alloc_info(api->GetDescriptorPool(), layouts);
		mDescriptorSets = std::move(vk::raii::DescriptorSets(mDevice, alloc_info));

		for (auto &[name, data] : shader_reflection.UniformBuffers)
		{
			mUniformBufferBindings.push_back(data.Binding);
		}
	}

	void VulkanBackendMaterial::Bind()
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &data)
		{
			const auto& pipeline_layout = mShader->GetPipelineLayout();
			std::vector<vk::WriteDescriptorSet> descriptor_writes;

			for (const auto &binding : mUniformBufferBindings)
			{
				auto ubo = std::dynamic_pointer_cast<VulkanUniformBuffer>(GlobalBuffers::GetUniformBuffer(binding));
				auto buffer_info = ubo->GetBufferInfo(data.Frame);
				
				vk::WriteDescriptorSet descriptor_write(mDescriptorSets[data.Frame], binding, 0, vk::DescriptorType::eUniformBuffer, {}, buffer_info);
				descriptor_writes.emplace_back(descriptor_write);
			}

			for (auto &[binding, texture] : mTextures)
			{
				auto image_info = *reinterpret_cast<const vk::DescriptorImageInfo *>(texture->GetNativeHandle());
				vk::WriteDescriptorSet descriptor_write(mDescriptorSets[data.Frame], binding, 0, vk::DescriptorType::eCombinedImageSampler, image_info);
				descriptor_writes.emplace_back(descriptor_write);
			}

			mDevice.updateDescriptorSets(descriptor_writes, {});
			data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, *mDescriptorSets[data.Frame], {});
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	void VulkanBackendMaterial::BindTexture(uint32_t binding, const Ref<Texture> &texture)
	{
		mTextures[binding] = texture;
	}

} // namespace BHive