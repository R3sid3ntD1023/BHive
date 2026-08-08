#include "DescriptorPoolManager.h"

namespace BHive
{
	vk::DescriptorPool DescriptorPoolManager::GetPool() const
	{
		return mPool;
	}

	void DescriptorPoolManager::Init(vk::raii::Device &device)
	{
		static vk::DescriptorPoolCreateFlags poolFlags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		// --- Material pool (set = 1, cached per material) ---
		std::vector<vk::DescriptorPoolSize> materialSizes
			= {{vk::DescriptorType::eCombinedImageSampler, 4096},
			   {vk::DescriptorType::eSampledImage, 4096},
			   {vk::DescriptorType::eStorageImage, 2048},
			   {vk::DescriptorType::eUniformBuffer, 1024},
			   {vk::DescriptorType::eStorageBuffer, 1024}};

		vk::DescriptorPoolCreateInfo matInfo{poolFlags, 4096, materialSizes};

		mPool = device.createDescriptorPool(matInfo);
	}

	void DescriptorPoolManager::Shutdown()
	{
	}
} // namespace BHive