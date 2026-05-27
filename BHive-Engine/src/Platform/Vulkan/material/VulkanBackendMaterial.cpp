#include "VulkanBackendMaterial.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/Texture.h"
#include "gfx/BufferBase.h"
#include "gfx/shader/ShaderProgram.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/Buffers.h"
#include "../systems/MaterialSetRegistry.h"
#include "Platform/Vulkan/VulkanSetManager.h"

namespace BHive
{
	
	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanBackendMaterial::Init(const Ref<Pipeline> &pipeline)
	{	
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		mBindPoint = vkPipeline->GetBindPoint();

		mProgram = Cast<ShaderProgram>(vkPipeline->GetShaderProgram());

		mReflectionMergedPtr = &mProgram->GetMergedRefl();
		mReflectionLookupTablePtr = &mProgram->GetRefl();

		//init set manager

		if (mReflectionMergedPtr->Sets.contains(MATERIAL_SET_INDEX))
		{
			mTargetSet = mReflectionMergedPtr->Sets.at(MATERIAL_SET_INDEX);

			// create local buffers
			for (auto &[name, ubo] : mTargetSet.UniformBuffers)
			{

				mLocalBuffers.emplace(name, GPUBuffer::Create(ubo.Size, EBufferType::UniformBuffer));
			}

			for (auto &[name, ssbo] : mTargetSet.StorageBuffers)
			{
				mLocalBuffers.emplace(name, GPUBuffer::Create(ssbo.Size, EBufferType::StorageBuffer));
			}

			GetSubSystem<MaterialSetRegistry>().CreateForMaterial(this, vkPipeline.get());
		}

		//create push constant buffer
		size_t total_size = 0;
		for (auto &pc : mReflectionMergedPtr->PushConstants)
			total_size = std::max(total_size, (size_t)pc.Offset + pc.Size);

		mPushConstantData.resize(total_size);
	}

	void VulkanBackendMaterial::Bind(const Ref<Pipeline> & pipeline)
	{
		auto vk_Pipeline = Cast<VulkanPipeline>(pipeline);
		auto &pipeline_layout = vk_Pipeline->GetLayout();
		auto manager = GetSubSystem<MaterialSetRegistry>().Find(this, vk_Pipeline.get());

		//take snapshot of current push data - copy by value
		auto pushData = mPushConstantData;

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Update MaterialSets",
			[=,&pipeline_layout](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				if (manager)
				{
					auto set = manager->GetNativeSet(vk_ctx.Frame).As<vk::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(mBindPoint, pipeline_layout, MATERIAL_SET_INDEX, *set, {});
				}
				
				//Update push constants
				for (auto &pc : mReflectionMergedPtr->PushConstants)
				{
					vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size, pushData.data() + pc.Offset);
					vk_ctx.CommandBuffer.pushConstants2(push_info);
				}
			});
	}

	void VulkanBackendMaterial::BindImmediate(vk::CommandBuffer cmd, const Ref<Pipeline> &pipeline)
	{
		auto vk_Pipeline = Cast<VulkanPipeline>(pipeline);
		auto &pipeline_layout = vk_Pipeline->GetLayout();
		auto manager = GetSubSystem<MaterialSetRegistry>().Find(this, vk_Pipeline.get());

		if (manager)
		{
			auto set = manager->GetNativeSet(0).As<vk::DescriptorSet>();
			cmd.bindDescriptorSets(mBindPoint, pipeline_layout, MATERIAL_SET_INDEX, *set, {});
		}

		// Update push constants
		for (auto &pc : mReflectionMergedPtr->PushConstants)
		{
			vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size, mPushConstantData.data() + pc.Offset);
			cmd.pushConstants2(push_info);
		}
	}

	void VulkanBackendMaterial::BindTextureImmediate(const std::string &name, const Ref<Texture> &texture, uint32_t mip, const Ref<Pipeline> &pipeline)
	{
		if (!texture)
			return;

		if (!mTargetSet.Samplers.contains(name))
		{
			LOG_ERROR("VulkanBackendMaterial::BindTexture - No sampler reflection for name {}", name);
			return;
		}

		auto &sampler = mTargetSet.Samplers.at(name);

		auto &registry = GetSubSystem<MaterialSetRegistry>();
		Cast<VulkanSetManager>(registry.Find(this, pipeline.get()))->SetTextureImmediate(sampler.Binding, texture, mip);
	}

	void VulkanBackendMaterial::BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip, const Ref<Pipeline>& pipeline)
	{
		if (!texture)
			return;

		if (!mTargetSet.Samplers.contains(name))
		{
			LOG_ERROR("VulkanBackendMaterial::BindTexture - No sampler reflection for name {}", name);
			return;
		}

		auto &sampler = mTargetSet.Samplers.at(name);

		auto &registry = GetSubSystem<MaterialSetRegistry>();
		registry.Find(this, pipeline.get())->SetTexture(sampler.Binding, texture, mip);
	}

	
	void VulkanBackendMaterial::Set(const std::string &name, const void *data, size_t size)
	{
		ASSERT(mReflectionMergedPtr)

		for (auto& pc : mReflectionMergedPtr->PushConstants)
		{
			if (pc.Members.contains(name))
			{
				const auto &u = pc.Members.at(name);

				memcpy(mPushConstantData.data() + u.Offset, data, size);
				return;
			}
		}

		for (auto& [ubo_name, ub] : mTargetSet.UniformBuffers)
		{
			if (ub.Members.contains(name))
			{
				auto &u = ub.Members.at(name);
				auto ubo = mLocalBuffers.at(ubo_name);
				ubo->SetData(data, size, u.Offset);
				return;
			}
		}

		for (auto &[name, ssb] : mTargetSet.StorageBuffers)
		{
			auto ssbo = mLocalBuffers.at(name);
			ssbo->SetData(data, size);
			return;
		}

		LOG_ERROR("Uniform '{}' not found in shader '{}'", name, mProgram->GetName());
	}

	void VulkanBackendMaterial::Shutdown()
	{
		LOG_TRACE("Shutdown VulkanBackendMaterial Called")
	}
}