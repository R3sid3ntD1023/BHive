#include "VulkanBackendMaterial.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/Texture.h"
#include "gfx/BufferBase.h"
#include "gfx/shader/ShaderProgram.h"
#include "Platform/Vulkan/VulkanConverters.h"
#include "renderers/Renderer.h"
#include "Platform/Vulkan/IVulkanTexture.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "gfx/shader/ShaderReflection.h"
#include "Platform/Vulkan/textures/VulkanImage.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"

namespace BHive
{
	
	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanBackendMaterial::Init(const Ref<Pipeline> &pipeline)
	{
		
		auto vkPipeline = Cast<VulkanPipeline>(pipeline);
		auto& shader = vkPipeline->GetVulkanShader();
		auto material_set_layout = shader.GetDescriptorSetLayout(MATERIAL_SET_INDEX);

		if (!material_set_layout)
			return;

		mProgram = Cast<ShaderProgram>(vkPipeline->GetShaderProgram());
		mReflectionPtr = &mProgram->GetRefl();
		mTargetSet = mReflectionPtr->Sets.at(MATERIAL_SET_INDEX);

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();

		mMaterialSetManager = CreateScope<SetManager>(mTargetSet, MATERIAL_SET_INDEX);
		mMaterialSetManager->Init(mDevice, api->GetDescriptorPool(), material_set_layout);

		// create local buffers
		for (auto& [name, ubo] : mTargetSet.UniformBuffers)
		{
			mLocalUBOs.emplace(name, UniformBuffer::Create(ubo.Binding, ubo.Size));
		}

		for (auto& [name, ssbo] : mTargetSet.StorageBuffers)
		{
			mLocalSSBOs.emplace(name, StorageBuffer::Create(ssbo.Binding, ssbo.Size));
		}
			

		vkPipeline->SetMaterialSet(mMaterialSetManager.get());
	
		//create push constant buffer
		size_t total_size = 0;
		for (auto &pc : mReflectionPtr->PushConstants)
			total_size = std::max(total_size, (size_t)pc.Offset + pc.Size);

		mPushConstantData.resize(total_size);
	}

	void VulkanBackendMaterial::Bind(const Ref<Pipeline> & pipeline)
	{
		if (!mMaterialSetManager)
			return;

		auto &pipeline_layout = Cast<VulkanPipeline>(pipeline)->GetLayout();

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Update MaterialSets",
			[=, &pipeline_layout](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				for (auto& [name, ub] : mTargetSet.UniformBuffers)
				{
					auto ubo = mLocalUBOs.at(name); 
					auto buffer_info = ubo->GetNativeHandle().As<vk::DescriptorBufferInfo>();
					if (buffer_info)
						mMaterialSetManager->BindBuffer(ub.Binding, vk::DescriptorType::eUniformBuffer, *buffer_info);
				}

				for (auto &[name, ssb] : mTargetSet.StorageBuffers)
				{
					auto ssbo = mLocalSSBOs.at(name);
					auto buffer_info = ssbo->GetNativeHandle().As<vk::DescriptorBufferInfo>();
					if (buffer_info)
						mMaterialSetManager->BindBuffer(ssb.Binding,vk::DescriptorType::eStorageBuffer ,*buffer_info);
				}

				mMaterialSetManager->Update(vk_ctx.Frame, mDevice);

				//Update push constants
				for (auto &pc : mReflectionPtr->PushConstants)
				{
					vk::PushConstantsInfo push_info(*pipeline_layout, ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size, mPushConstantData.data() + pc.Offset);
					vk_ctx.CommandBuffer.pushConstants2(push_info);
				}
			});
	}

	void VulkanBackendMaterial::BindTexture(const std::string& name, const Ref<Texture> &texture)
	{
		if (!texture || !mMaterialSetManager)
			return;

		if (!mTargetSet.Samplers.contains(name))
		{
			LOG_ERROR("VulkanBackendMaterial::BindTexture - No sampler reflection for name {}", name);
			return;
		}

		auto &sampler = mTargetSet.Samplers.at(name);
		auto image = texture->GetNativeHandle().As<AllocatedImage>();
		vk::DescriptorImageInfo image_info = image->GetDescriptor();
		mMaterialSetManager->BindSampler(sampler.Binding, image_info);
	}

	
	void VulkanBackendMaterial::Set(const std::string &name, const void *data, size_t size)
	{
		auto &refl = mProgram->GetRefl();

		for (auto& pc : refl.PushConstants)
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