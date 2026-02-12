#include "core/Application.h"
#include "core/FileSystem.h"
#include "core/subsystem/SubSystem.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "gfx/utils/shader/ShaderCompiler.h"
#include "gfx/utils/shader/ShaderSerializer.h"
#include "gfx/utils/shader/ShaderTimeCache.h"
#include "gfx/utils/shader/ShaderUtils.h"
#include "VulkanWindowContext.h"
#include "VulkanSwapChain.h"
#include "VulkanUniformBuffer.h"
#include "renderers/buffers/GlobalBuffers.h"
#include "VulkanPipeline.h"
#include "VulkanRendererAPI.h"
#include "VulkanShader.h"

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
		: mDevice(VulkanBackend::GetLogicalDevice()),
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

		CompileFromSource();
	}

	VulkanShader::VulkanShader(const std::string &name, const std::string &vert, const std::string &frag, const FRenderOptions &options)
		: mDevice(VulkanBackend::GetLogicalDevice()),
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
		//LOG_TRACE("Current Pipeline Bound : {}", (void*)mGraphicsPipeline.get());
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
		auto swap_chain = static_cast<VulkanWindowContext &>(WindowContext::Get()).GetSwapChain();
		auto format = swap_chain->GetFormat().format;
	
		//LOG_TRACE("Pipeline RenderInfo Format : {}", vk::to_string(format));

		std::vector<vk::PipelineShaderStageCreateInfo> create_infos;

		for (auto &[stage, module] : mShaderModules)
		{
			create_infos.emplace_back(vk::PipelineShaderStageCreateFlags{}, utils::GetAPIShaderStage(stage), module, "main");
			//LOG_TRACE("Creating pipeline with module {}", (void*) & module);
		}

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, *mDescriptorSetLayout);
		mPipelineLayout = mDevice.createPipelineLayout(pipeline_layout_create_info);

		vk::PipelineRenderingCreateInfo rendering_info{};
		rendering_info.setViewMask(0).setColorAttachmentCount(1).setColorAttachmentFormats(format);


		auto config = VulkanPipeline::GetDefaultConfigInfo();
		config->Layout = mPipelineLayout;
		config->Next = &rendering_info;
		config->ShaderCreateInfos = create_infos;
		config->InputAssembly.setTopology(utils::GetTopology(mRenderOptions.DrawMode));
		config->Rasterazation.setCullMode(utils::GetCullMode(mRenderOptions.CullMode));
		config->DepthStencil.setDepthTestEnable(VK_FALSE).setDepthWriteEnable(VK_FALSE).setDepthBoundsTestEnable(VK_FALSE).setStencilTestEnable(VK_FALSE);
			//.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
	//	config.DepthStencil.setDepthTestEnable(VK_FALSE).setDepthWriteEnable(VK_FALSE);

		mGraphicsPipeline = Pipeline::Create();
		mGraphicsPipeline->Init(config);

		
	}

	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		
	}

	void VulkanBackendMaterial::Init(const Ref<Shader> &shader)
	{	
		auto vulkan_shader = Cast<VulkanShader>(shader);
		
		ASSERT(vulkan_shader)

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		std::vector<vk::DescriptorSetLayout> layouts(VulkanBackend::MAX_FRAMES_IN_FLIGHT, vulkan_shader->GetDescriptorSetLayout());
		vk::DescriptorSetAllocateInfo alloc_info(api->GetDescriptorPool(), layouts);
		mDescriptorSets = std::move(vk::raii::DescriptorSets(mDevice, alloc_info));

		const auto &shader_reflection = vulkan_shader->GetRelectionData();

		for (auto &[name, data] : shader_reflection.UniformBuffers)
		{
			mUniformBufferBindings.push_back(data.Binding);
		}
	}

	void VulkanBackendMaterial::Bind(const Ref<Shader> &shader)
	{
		auto vulkan_shader = Cast<VulkanShader>(shader);

		ASSERT(vulkan_shader)

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto pre_cmd = [=](const FVulkanFrameData &data)
		{
			
			const auto &descriptor_set = mDescriptorSets[data.Frame];

			std::vector<vk::WriteDescriptorSet> descriptor_writes;

			for (const auto &binding : mUniformBufferBindings)
			{
				auto ubo = std::dynamic_pointer_cast<VulkanUniformBuffer>(GlobalBuffers::GetUniformBuffer(binding));
				auto buffer_info = *ubo->GetNativeHandle(data.Frame).As<vk::DescriptorBufferInfo>();
				
				vk::WriteDescriptorSet descriptor_write(descriptor_set, binding, 0, vk::DescriptorType::eUniformBuffer, {}, buffer_info);
				descriptor_writes.emplace_back(descriptor_write);
			}

			mDevice.updateDescriptorSets(descriptor_writes, {});
			
		};

		api->SubmitCommand(pre_cmd, ECommandType_PreCommand);

		auto cmd = [=](const FVulkanFrameData &data)
		{
			const auto &pipeline_layout = vulkan_shader->GetPipelineLayout();
			data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, *mDescriptorSets[data.Frame], {});
		};

		api->SubmitCommand(cmd);
	}

	void VulkanBackendMaterial::BindTexture(uint32_t binding, const Ref<Texture> &texture)
	{
		if (!texture)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto pre_cmd = [=](const FVulkanFrameData &data)
		{
			const auto &descriptor_set = mDescriptorSets[data.Frame];
			vk::DescriptorImageInfo image_info = *texture->GetNativeHandle().As<vk::DescriptorImageInfo>();
			vk::WriteDescriptorSet descriptor_write(descriptor_set, binding, 0, vk::DescriptorType::eCombinedImageSampler, image_info);
			mDevice.updateDescriptorSets(descriptor_write, {});
		};

		api->SubmitCommand(pre_cmd, ECommandType_PreCommand);
	}

	void VulkanBackendMaterial::Shutdown()
	{
		LOG_TRACE("Shutdown VulkanBackendMaterial Called")
	}

} // namespace BHive