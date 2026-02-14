#include "VulkanBackendMaterial.h"
#include "renderers/buffers/GlobalBuffers.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace BHive
{
	VulkanBackendMaterial::VulkanBackendMaterial()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
	}

	void VulkanBackendMaterial::Init(const Ref<Pipeline> &pipeline)
	{
		auto shader = Cast<VulkanShader>(pipeline->GetShader());
		auto& descriptor_set_layout = shader->GetDescriptorSetLayout();

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		std::vector<vk::DescriptorSetLayout> layouts(VulkanBackend::MAX_FRAMES_IN_FLIGHT, descriptor_set_layout);
		vk::DescriptorSetAllocateInfo alloc_info(api->GetDescriptorPool(), layouts);
		mDescriptorSets = std::move(vk::raii::DescriptorSets(mDevice, alloc_info));

		const auto &shader_reflection = shader->GetRefl();

		for (auto &[name, data] : shader_reflection.UniformBuffers)
		{
			mUniformBufferBindings.push_back(data.Binding);
		}
	}

	void VulkanBackendMaterial::Bind(const Ref<Pipeline> & pipeline)
	{

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
			const auto &pipeline_layout = Cast<VulkanPipeline>(pipeline)->GetLayout();
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
}