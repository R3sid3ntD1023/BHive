#include "SetManager.h"
#include "VulkanBackend.h"
#include "gfx/GlobalBuffers.h"
#include "core/subsystem/SubSystem.h"

namespace BHive
{

	SetManager::SetManager(const FSetReflection &set, uint32_t setIndex)
		: mSetIndex(setIndex),
		  mTargetSet(set)
	{
	}

	void SetManager::Init(vk::raii::Device &device, vk::raii::DescriptorPool &pool, vk::DescriptorSetLayout layout)
	{
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::DescriptorSetAllocateInfo alloc_info(pool, layout);
			auto sets = device.allocateDescriptorSets(alloc_info);
			mSets.emplace_back(std::move(sets[0]));
		}
	}

	void SetManager::BindBuffer(uint32_t binding, vk::DescriptorType type, vk::DescriptorBufferInfo buffer)
	{
		mLocalBuffers.emplace(binding, std::pair{type, buffer});
	}


	void SetManager::BindSampler(uint32_t binding, vk::DescriptorImageInfo imageInfo)
	{
		mLocalSamplers.emplace(binding, imageInfo);
	}

	void SetManager::Update(uint32_t frame, vk::raii::Device &device)
	{
		auto target = *mSets[frame];
		std::vector<vk::WriteDescriptorSet> writes;
		auto &global_buffers = GetSubSystem<GlobalBuffers>();

		//Global Buffers
		if (mSetIndex == GLOBAL_SET_INDEX)
		{
			for (auto &[name, ub] : mTargetSet.UniformBuffers)
			{
				if (!global_buffers.Contains(ub.Binding))
					continue;

				auto handle = global_buffers.GetBuffer(ub.Binding).GetHandle(frame);
				auto info = handle.As<vk::DescriptorBufferInfo>();
				if (!info)
					continue;

				writes.emplace_back(target, ub.Binding, 0, vk::DescriptorType::eUniformBuffer, VK_NULL_HANDLE, *info);
			}
		}

		for (auto &[binding, info] : mLocalBuffers)
			writes.emplace_back(target, binding, 0, info.first, VK_NULL_HANDLE, info.second);

		for (auto& [binding, info] : mLocalSamplers)
		{
			if (info.imageView == VK_NULL_HANDLE || info.sampler == VK_NULL_HANDLE)
				continue;

			writes.emplace_back(target, binding, 0, vk::DescriptorType::eCombinedImageSampler, info);
		}
		
		if (!writes.empty())
		{
			device.updateDescriptorSets(writes, {});
		}
	}
	
	void GlobalSetSystem::Register(uint64_t hash, Scope<SetManager>& manager)
	{
		mGlobalManagers[hash] = std::move(manager);
	}

	SetManager *GlobalSetSystem::Get(uint64_t hash) const
	{
		return mGlobalManagers.at(hash).get();
	}

} // namespace BHive