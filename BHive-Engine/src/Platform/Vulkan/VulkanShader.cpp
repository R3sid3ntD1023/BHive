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
		DestroyDescriptorResources();
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
		UpdateDescriptorResources();

		mGraphicsPipeline->Bind();

		if (mDescriptorSets.size())
		{
			auto cmd = [=](const FVulkanFrameData &data) 
				{
				//LOG_INFO("CMD: Shader::Bind, frame={}", data.Frame);
				ASSERT(data.Frame < mDescriptorSets.size());
				data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, *mDescriptorSets[data.Frame], {});
			};

			RenderCommand::GetAPI<VulkanRendererAPI>()->SubmitCommand(cmd);
		}
	}

	void VulkanShader::UnBind()
	{
	}

	void VulkanShader::Dispatch(uint32_t w, uint32_t h, uint32_t d)
	{
	}

	void VulkanShader::BindTexture(uint32_t binding, const Ref<Texture> &texture)
	{
		if (!texture)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &data)
		{
			auto image_info = *reinterpret_cast<const vk::DescriptorImageInfo *>(texture->GetNativeHandle());
			//FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).WriteImage(binding, *image_info).Overwrite(mDescriptorSets[data.Frame]);

			vk::WriteDescriptorSet descriptor_write(mDescriptorSets[data.Frame], binding, 0, vk::DescriptorType::eCombinedImageSampler, image_info);
			mDevice.updateDescriptorSets(descriptor_write, {});
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
		
	}

	void VulkanShader::CreateDescriptorResources()
	{
		DestroyDescriptorResources();

		auto num_samplers = (uint32_t)mReflectionData.Samplers.size();
		auto num_uniform_buffers = (uint32_t)mReflectionData.UniformBuffers.size();
		auto max_sets = (num_samplers + num_uniform_buffers) * VulkanCore::MAX_FRAMES_IN_FLIGHT;

		for (auto &[name, data] : mReflectionData.UniformBuffers)
		{
			mUniformBufferBindings.push_back(data.Binding);
		}

		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		std::vector<vk::DescriptorPoolSize> pool_sizes;
		//FDescriptorSetLayout::Builder builder;
		for (auto &[name, sampler] : mReflectionData.Samplers)
		{
			//builder.AddBinding(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1);
			bindings.emplace_back(sampler.Binding, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);
			pool_sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}

		for (auto &[name, uniform_buffer] : mReflectionData.UniformBuffers)
		{
			//builder.AddBinding(uniform_buffers.Binding, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, 1);
			bindings.emplace_back(uniform_buffer.Binding, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
			pool_sizes.emplace_back(vk::DescriptorType::eUniformBuffer, VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}

		vk::DescriptorSetLayoutCreateInfo layout_info({}, bindings, nullptr);
		mDescriptorSetLayout = mDevice.createDescriptorSetLayout(layout_info);

		vk::DescriptorPoolCreateInfo pool_create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, VulkanCore::MAX_FRAMES_IN_FLIGHT, pool_sizes);
		mDescriptorPool = mDevice.createDescriptorPool(pool_create_info);

		std::vector<vk::DescriptorSetLayout> layouts(VulkanCore::MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayout);
		vk::DescriptorSetAllocateInfo alloc_info(mDescriptorPool, layouts);
		mDescriptorSets = std::move(vk::raii::DescriptorSets(mDevice, alloc_info));

		/*mDescriptorSetLayout = builder.Build();

		if (max_sets == 0)
			return;

		FDescriptorPool::Builder pool_builder;
		pool_builder.SetMaxSets(max_sets);

		if (num_samplers)
		{
			pool_builder.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, num_samplers * VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}


		if (num_uniform_buffers)
		{
			pool_builder.AddPoolSize(vk::DescriptorType::eUniformBuffer, num_uniform_buffers * VulkanCore::MAX_FRAMES_IN_FLIGHT);
		}

		mDescriptorPool = pool_builder.Build();

		FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).Build(mDescriptorSets);

		ASSERT(mDescriptorSets.size() == VulkanCore::MAX_FRAMES_IN_FLIGHT)*/
	}

	void VulkanShader::DestroyDescriptorResources()
	{
		mDescriptorSets.clear();
		mDescriptorPool = nullptr;
	}

	void VulkanShader::UpdateDescriptorResources()
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &data)
		{
			ASSERT(data.Frame < mDescriptorSets.size());

			for (const auto &binding : mUniformBufferBindings)
			{
				auto ubo = std::dynamic_pointer_cast<VulkanUniformBuffer>(GlobalBuffers::GetUniformBuffer(binding));
				auto buffer_info = ubo->GetBufferInfo(data.Frame);
				//FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).WriteBuffer(binding, buffer_info).Overwrite(mDescriptorSets[data.Frame]);
				vk::WriteDescriptorSet descriptor_write(mDescriptorSets[data.Frame], binding, 0, vk::DescriptorType::eUniformBuffer, {}, buffer_info);
				mDevice.updateDescriptorSets(descriptor_write, {});
			}
		};
		
		api->SubmitCommand(cmd, ECommandType_PreCommand);
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

} // namespace BHive