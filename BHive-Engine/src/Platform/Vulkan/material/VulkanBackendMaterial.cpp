#include "VulkanBackendMaterial.h"
#include "renderers/buffers/GlobalBuffers.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanStorageBuffer.h"
#include "gfx/shader/ShaderProgram.h"
#include "Platform/Vulkan/VulkanConverters.h"

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
		auto& descriptor_set_layouts = vkPipeline->GetDescriptorLayouts();

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		for (uint32_t frame = 0; frame < VulkanBackend::MAX_FRAMES_IN_FLIGHT; frame++)
		{
			vk::DescriptorSetAllocateInfo alloc_info(api->GetDescriptorPool(), descriptor_set_layouts);
			mDescriptorSets.emplace_back(std::move(vk::raii::DescriptorSets(mDevice, alloc_info)));
		}
		
		mReflectionPtr = &mProgram->GetRefl();

		ASSERT(mReflectionPtr);

		size_t total_size = 0;
		for (auto &pc : mReflectionPtr->PushConstants)
			total_size = std::max(total_size, (size_t)pc.Offset + pc.Size);

		mPushConstantData.resize(total_size);
	}

	void VulkanBackendMaterial::Bind(const Ref<Pipeline> & pipeline)
	{
		auto &pipeline_layout = Cast<VulkanPipeline>(pipeline)->GetLayout();

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto pre_cmd = [=](const FVulkanFrameData &data)
		{
			const auto &frame_set = mDescriptorSets[data.Frame];

			std::vector<vk::WriteDescriptorSet> descriptor_writes;

			for (const auto &[name, ub] : mReflectionPtr->UniformBuffers)
			{
				auto& target_set = frame_set[ub.Set];
				auto ubo = Cast<VulkanUniformBuffer>(GlobalBuffers::GetUniformBuffer(ub.Binding));
				auto buffer_info = *ubo->GetNativeHandle(data.Frame).As<vk::DescriptorBufferInfo>();

				vk::WriteDescriptorSet descriptor_write(target_set, ub.Binding, 0, vk::DescriptorType::eUniformBuffer, {}, buffer_info);
				descriptor_writes.emplace_back(descriptor_write);
			}

			for (const auto &[name,sb] : mReflectionPtr->StorageBuffers)
			{
				auto &target_set = frame_set[sb.Set];
				auto sbo = Cast<VulkanStorageBuffer>(GlobalBuffers::GetStorageBuffer(sb.Binding));
				auto buffer_info = *sbo->GetNativeHandle(data.Frame).As<vk::DescriptorBufferInfo>();

				vk::WriteDescriptorSet descriptor_write(target_set, sb.Binding, 0, vk::DescriptorType::eStorageBuffer, {}, buffer_info);
				descriptor_writes.emplace_back(descriptor_write);
			}

			mDevice.updateDescriptorSets(descriptor_writes, {});
		};

		api->SubmitCommand(pre_cmd, ECommandType_PreCommand);

		auto cmd = [this, &pipeline_layout](const FVulkanFrameData &data)
		{ 
			const auto &frame_sets = mDescriptorSets[data.Frame];
			std::vector<vk::DescriptorSet> raw_sets;
			raw_sets.reserve(frame_sets.size());
			for (auto& s : frame_sets)
			{
				raw_sets.push_back(s);
			}

			data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, raw_sets, {});

			if (mPushConstantDirty)
			{
				for (auto& pc : mReflectionPtr->PushConstants)
				{
					vk::PushConstantsInfo push_info(*pipeline_layout, Vulkan::ToVkShaderStageBit(pc.Stages), pc.Offset, pc.Size, mPushConstantData.data() + pc.Offset);
					data.CommandBuffer.pushConstants2(push_info);
				}

				mPushConstantDirty = false;
			}
		};

		api->SubmitCommand(cmd);
	}

	void VulkanBackendMaterial::BindTexture(const std::string& name, const Ref<Texture> &texture)
	{
		if (!texture)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto pre_cmd = [=](const FVulkanFrameData &data)
		{
			const auto &frame_set = mDescriptorSets[data.Frame];

			if (!mReflectionPtr->Samplers.contains(name))
			{
				LOG_ERROR("VulkanBackendMaterial::BindTexture - No sampler reflection for name {}", name);
				return;
			}

			auto &sampler = mReflectionPtr->Samplers.at(name);
			uint32_t setIndex = sampler.Set;
			if (setIndex >= frame_set.size())
			{
				LOG_ERROR("VulkanBackendMaterial::BindTexture - Set index {} out of range", setIndex);
				return;
			}

			const vk::DescriptorSet& target_set = frame_set[setIndex];

			vk::DescriptorImageInfo image_info = *texture->GetNativeHandle().As<vk::DescriptorImageInfo>();
			vk::WriteDescriptorSet descriptor_write(target_set, sampler.Binding, 0, vk::DescriptorType::eCombinedImageSampler, image_info);
			mDevice.updateDescriptorSets(descriptor_write, {});
		};

		api->SubmitCommand(pre_cmd, ECommandType_PreCommand);
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
				mPushConstantDirty = true;
				return;
			}
		}

		for (auto& [stage, ub] : refl.UniformBuffers)
		{
			if (ub.Members.contains(name))
			{
				auto &u = ub.Members.at(name);
				auto ubo = GlobalBuffers::GetUniformBuffer(ub.Binding);
				ubo->SetData(data, size, u.Offset);
				return;
			}
		}

		LOG_ERROR("Uniform '{}' not found in shader '{}'", name, mProgram->GetName());
	}

	void VulkanBackendMaterial::Shutdown()
	{
		LOG_TRACE("Shutdown VulkanBackendMaterial Called")
	}
}