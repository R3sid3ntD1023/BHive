#include "VulkanBackendMaterial.h"
#include "VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "gfx/Texture.h"
#include "gfx/BufferBase.h"
#include "gfx/shader/ShaderProgram.h"
#include "VulkanConversions.h"
#include "VulkanBackend.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/Buffers.h"
#include "VulkanShader.h"
#include "VulkanBindingGroup.h"

namespace BHive
{
	
	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanBackendMaterial::Init(Pipeline* pipeline)
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
		}

		//create push constant buffer
		size_t total_size = 0;
		for (auto &pc : mReflectionMergedPtr->PushConstants)
			total_size = std::max(total_size, (size_t)pc.Offset + pc.Size);

		mPushConstantData.resize(total_size);
	}

	void VulkanBackendMaterial::Bind(Pipeline* pipeline)
	{
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		auto &pipeline_layout = vkPipeline->GetLayout();

		//take snapshot of current push data - copy by value
		auto pushData = mPushConstantData;

		BindToPipeline(vkPipeline);

		pipeline->Bind();

		if (vkPipeline->HasSet(MATERIAL_SET_INDEX))
		{
			// bind cached material descriptor set
			auto group = Cast<VulkanBindingGroup>(vkPipeline->GetOrCreateBindingGroup(MATERIAL_SET_INDEX));
			auto matSet = group->GetOrCreateMaterialSet();


			RenderCommand::SubmitCommand(
				"Bind Material Set",
				[=](IRendererContext &ctx)
				{
					auto &vk_ctx = ctx.As<FVulkanRendererContext>();
					vk_ctx.CommandBuffer.bindDescriptorSets(vkPipeline->GetBindPoint(), vkPipeline->GetLayout(), MATERIAL_SET_INDEX, matSet, {});
				});
		
		}
		
		RenderCommand::SubmitCommand(
			"Update PushConstants",
			[=,&pipeline_layout](IRendererContext &ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanRendererContext>();

				//Update push constants
				for (auto &pc : mReflectionMergedPtr->PushConstants)
				{
					vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, (uint32_t)pc.Size, pushData.data() + pc.Offset);
					vk_ctx.CommandBuffer.pushConstants2(push_info);
				}
			});
	}

	void VulkanBackendMaterial::BindImmediate(vk::CommandBuffer cmd, Pipeline* pipeline)
	{
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		auto &pipeline_layout = vkPipeline->GetLayout();

		BindToPipeline(vkPipeline);

		vkPipeline->BindImmediate(cmd);

		if (vkPipeline->HasSet(MATERIAL_SET_INDEX))
		{
			// bind cached material descriptor set
			auto group = Cast<VulkanBindingGroup>(vkPipeline->GetOrCreateBindingGroup(MATERIAL_SET_INDEX));
			auto matSet = group->GetOrCreateMaterialSet();

			cmd.bindDescriptorSets(vkPipeline->GetBindPoint(), vkPipeline->GetLayout(), MATERIAL_SET_INDEX, matSet, {});
		}

		// Update push constants
		for (auto &pc : mReflectionMergedPtr->PushConstants)
		{
			vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, (uint32_t)pc.Size, mPushConstantData.data() + pc.Offset);
			cmd.pushConstants2(push_info);
		}
	}

	void VulkanBackendMaterial::BindTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip, Pipeline* pipeline)
	{
		if (!texture)
			return;

		if (!mTargetSet.Samplers.contains(name))
		{
			LOG_ERROR("VulkanBackendMaterial::BindTexture - No sampler reflection for name {}", name);
			return;
		}

		
		auto &smp = mTargetSet.Samplers.at(name);
		auto set = Cast<VulkanPipeline>(pipeline)->GetOrCreateBindingGroup(MATERIAL_SET_INDEX);
		set->SetTexture(smp.Binding, texture, mip);
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
				auto& ubo = mLocalBuffers.at(ubo_name);
				ubo->SetData(data, size, u.Offset);
				return;
			}
		}

		if (mTargetSet.StorageBuffers.contains(name))
		{
			auto &ssbo = mLocalBuffers.at(name);
			ssbo->SetData(data, size);
			return;
		}

		LOG_ERROR("Uniform '{}' not found in shader '{}'", name, mProgram->GetName());
	}

	void VulkanBackendMaterial::BindToPipeline(VulkanPipeline *pipeline)
	{
		auto& shader = pipeline->GetVulkanShader();
		if (!shader.HasSet(MATERIAL_SET_INDEX))
			return;

		auto set = pipeline->GetOrCreateBindingGroup(MATERIAL_SET_INDEX);
		auto& setBindings = mReflectionLookupTablePtr->GetSetBindings(MATERIAL_SET_INDEX);

		for (auto& r : setBindings)
		{
			auto it = mLocalBuffers.find(r.name);
			if (it != mLocalBuffers.end() && it->second)
			{
				set->SetBuffer(r.binding, it->second);
				continue;
			}
		}
		//TODO: textures maybe
	}

}