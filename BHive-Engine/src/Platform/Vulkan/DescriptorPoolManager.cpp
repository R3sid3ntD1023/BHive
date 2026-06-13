#include "DescriptorPoolManager.h"

namespace BHive
{
	vk::DescriptorPool DescriptorPoolManager::GetPool(uint32_t set, uint32_t frame) const
	{
		if (set == MATERIAL_SET_INDEX)
			return MaterialPool;

		return FramePools[frame];
	}

	void DescriptorPoolManager::Init(vk::Device device)
	{
		Device = device;

		// --- Material pool (set = 1, cached per material) ---
		std::vector<vk::DescriptorPoolSize> materialSizes = {
			{vk::DescriptorType::eCombinedImageSampler, 4096},
			{vk::DescriptorType::eSampledImage, 4096},
			{vk::DescriptorType::eStorageImage, 2048},
			{vk::DescriptorType::eUniformBuffer, 1024},
			{vk::DescriptorType::eStorageBuffer, 1024}
		};

		vk::DescriptorPoolCreateInfo matInfo{{}, 4096, materialSizes};

		MaterialPool = Device.createDescriptorPool(matInfo);

		//---Engine Pool (BRDF, skybox, etc.) ---
		std::vector<vk::DescriptorPoolSize> engineSizes = {{vk::DescriptorType::eCombinedImageSampler, 64}, {vk::DescriptorType::eStorageImage, 64}};

		vk::DescriptorPoolCreateInfo engineInfo{{}, 128, engineSizes};

		EnginePool = Device.createDescriptorPool(engineInfo);

		//---Frame  Pools(globals, per-frame, per-pass) ---
		std::vector<vk::DescriptorPoolSize> frameSizes = {
			{vk::DescriptorType::eCombinedImageSampler, 128}, {vk::DescriptorType::eUniformBuffer, 256}, {vk::DescriptorType::eStorageBuffer, 256},
			{vk::DescriptorType::eSampledImage, 256},		  {vk::DescriptorType::eStorageImage, 256},
		};
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::DescriptorPoolCreateInfo frameInfo{{}, 512, frameSizes};

			FramePools[i] = Device.createDescriptorPool(frameInfo);
		}
	}

	void DescriptorPoolManager::ResetFrame(uint32_t frameIndex)
	{
		Device.resetDescriptorPool(FramePools[frameIndex]);
	}

	void DescriptorPoolManager::Shutdown()
	{
		for (auto &pool : FramePools)
			if (pool)
				Device.destroyDescriptorPool(pool);

		if (MaterialPool)
			Device.destroyDescriptorPool(MaterialPool);

		if (EnginePool)
			Device.destroyDescriptorPool(EnginePool);
	}
} // namespace BHive