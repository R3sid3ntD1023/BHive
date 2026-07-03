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
		auto group = Cast<VulkanPipeline>(pipeline)->GetOrCreateBindingGroup(MATERIAL_SET_INDEX);
		group->SetTexture(smp.Binding, texture, mip);

		mTextureBindings[name] = {smp.Binding, texture, mip};
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

	MaterialSnapshot VulkanBackendMaterial::CreateSnapshot() const
	{
		MaterialSnapshot snapshot;

		snapshot.LocalBuffers = mLocalBuffers;
		snapshot.PushConstantData = mPushConstantData;
		snapshot.Textures = mTextureBindings;

		return snapshot;
	}
}