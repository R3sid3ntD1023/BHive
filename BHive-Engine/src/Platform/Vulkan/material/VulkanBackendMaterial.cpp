#include "VulkanBackendMaterial.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/Texture.h"
#include "gfx/BufferBase.h"
#include "gfx/shader/ShaderProgram.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "gfx/shader/ShaderReflection.h"
#include "Platform/Vulkan/textures/VulkanImage.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"
#include "../systems/MaterialSetRegistry.h"

namespace BHive
{
	
	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanBackendMaterial::Init(const Ref<Pipeline> &pipeline)
	{
		
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);

		

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
				mLocalUBOs.emplace(name, UniformBuffer::Create(ubo.Binding, ubo.Size));
			}

			for (auto &[name, ssbo] : mTargetSet.StorageBuffers)
			{
				mLocalSSBOs.emplace(name, StorageBuffer::Create(ssbo.Binding, ssbo.Size));
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
		auto &pipeline_layout = Cast<VulkanPipeline>(pipeline)->GetLayout();
		auto manager = GetSubSystem<MaterialSetRegistry>().Find(this);

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Update MaterialSets",
			[=, &pipeline_layout](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				
				if (manager)
				{
					auto set = manager->GetNativeSet(vk_ctx.Frame).As<vk::DescriptorSet>();
					vk_ctx.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, MATERIAL_SET_INDEX, *set, {});
				}
				
				//Update push constants
				for (auto &pc : mReflectionMergedPtr->PushConstants)
				{
					vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size, mPushConstantData.data() + pc.Offset);
					vk_ctx.CommandBuffer.pushConstants2(push_info);
				}
			});
	}

	void VulkanBackendMaterial::BindTexture(const std::string& name, const Ref<Texture> &texture)
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
		registry.Find(this)->SetTexture(sampler.Binding, texture);
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
				auto ubo = mLocalUBOs.at(ubo_name);
				ubo->SetData(data, size, u.Offset);
				return;
			}
		}

		for (auto &[name, ssb] : mTargetSet.StorageBuffers)
		{
			auto ssbo = mLocalSSBOs.at(name);
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