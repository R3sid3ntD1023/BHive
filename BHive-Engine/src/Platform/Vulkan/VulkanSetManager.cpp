#include "VulkanSetManager.h"
#include "VulkanBackend.h"
#include "gfx/GlobalBuffers.h"
#include "core/subsystem/SubSystem.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConverters.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanSetManager::VulkanSetManager(vk::raii::Device &device, vk::raii::DescriptorPool& pool, vk::DescriptorSetLayout layout, uint32_t setIndex, const FShaderReflection &refl)
		: mDevice(device),
		  mSetIndex(setIndex),
		  mRefl(refl)
	{
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::DescriptorSetAllocateInfo alloc_info(pool, layout);
			auto sets = device.allocateDescriptorSets(alloc_info);
			mSets.emplace_back(std::move(sets[0]));
		}
	}

	void VulkanSetManager::BindBuffer(uint32_t binding, EResourceType type, const Ref<BufferBase> &buffer)
	{
		auto b = buffer->GetNativeHandle().As<AllocatedBuffer>();
		vk::DescriptorBufferInfo info(b->Buffer, 0, b->Size);
		auto type_ = ToVkType(type);
		mLocalBuffers.emplace(binding, std::pair{type_, info});
	}

	void VulkanSetManager::BindSampler(uint32_t binding, EResourceType type, const Ref<Texture> &texture)
	{
		vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal;
		auto vk_type = ToVkType(type);
		switch(vk_type)
		{
		case vk::DescriptorType::eStorageImage:
		{
			layout = vk::ImageLayout::eGeneral;
			break;
		}
		default:
			break;
		}
		auto img = texture->GetNativeHandle().As<AllocatedImage>();
		vk::DescriptorImageInfo info(img->GetSampler(), img->GetView(), layout); 
		mLocalSamplers.emplace(binding, info);
	}

	void VulkanSetManager::Update(uint32_t frame)
	{
		
		auto& global_buffers = GetSubSystem<GlobalBuffers>();

		auto target = *mSets[frame];

		auto &target_refl_set = mRefl.Sets.at(mSetIndex);

		std::vector<vk::WriteDescriptorSet> writes;

		// Global Buffers
		if (mSetIndex == GLOBAL_SET_INDEX)
		{
			for (auto &[name, ub] : target_refl_set.UniformBuffers)
			{
				if (!global_buffers.Contains(ub.Binding))
					continue;

				auto handle = global_buffers.GetBuffer(ub.Binding).GetHandle(frame);
				auto buffer = handle.As<AllocatedBuffer>();

				vk::DescriptorBufferInfo info(buffer->Buffer, 0, buffer->Size);
				writes.emplace_back(target, ub.Binding, 0, vk::DescriptorType::eUniformBuffer, VK_NULL_HANDLE, info);
			}
		}

		for (auto &[binding, info] : mLocalBuffers)
		{
			writes.emplace_back(target, binding, 0, info.first, VK_NULL_HANDLE, info.second);
		}

		for (auto &[binding, info] : mLocalSamplers)
		{
			if (info.imageView == VK_NULL_HANDLE || info.sampler == VK_NULL_HANDLE)
				continue;

			writes.emplace_back(target, binding, 0, vk::DescriptorType::eCombinedImageSampler, info);
		}

		if (!writes.empty())
		{
			mDevice.updateDescriptorSets(writes, {});
		}
	}

	NativeHandle VulkanSetManager::GetNativeSet(uint32_t frame)
	{
		return NativeHandle::FromPtr(&mSets[frame]);
	}
} // namespace BHive