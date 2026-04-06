#pragma once

#include "core/Core.h"

namespace BHive
{
	class VulkanBackendMaterial;
	class ISetManager;
	class Pipeline;
	struct FSetReflection;

	class MaterialSetRegistry
	{
		struct Key
		{
			VulkanBackendMaterial *Mat;
			const Pipeline *Pipeline;

			bool operator==(const Key &other) const { return Mat == other.Mat && Pipeline == other.Pipeline; }
		};

		struct KeyHash
		{
			size_t operator()(const Key &k) const { return std::hash<void *>()(k.Mat) ^ (std::hash<const void *>()(k.Pipeline) << 1); }
		};

		struct Entry
		{
			VulkanBackendMaterial *Owner = nullptr;
			uint64_t SetHash = 0;
			uint64_t SetIndex = 0;
			Ref<ISetManager> Manager;
		};

	public:
		Entry &CreateForMaterial(VulkanBackendMaterial *mat, const Pipeline *pipeline);

		Ref<ISetManager> Find(VulkanBackendMaterial *mat, Pipeline *pipeline) const;

		void UpdatePerFrame(uint32_t frame);

	private:
		void BindMaterialResources(VulkanBackendMaterial *mat, const FSetReflection &refl, ISetManager *mang);

	private:
		std::unordered_map<Key, Entry, KeyHash> mEntries;
	};

} // namespace BHive