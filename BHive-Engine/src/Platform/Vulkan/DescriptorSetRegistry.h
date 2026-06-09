#pragma once

#include "core/Core.h"

namespace BHive
{
	class VulkanBackendMaterial;
	class DescriptorSetManager;
	class VulkanPipeline;
	struct FSetReflection;
	class GPUBuffer;


	class DescriptorSetRegistry
	{
		
	public:

		static DescriptorSetRegistry &Get()
		{
			static DescriptorSetRegistry sInstance;
			return sInstance;
		}

		Ref<DescriptorSetManager> GetSet(const VulkanPipeline& pipeline, uint32_t setIndex, VulkanBackendMaterial *backendMat, uint32_t frame);

	private:
		DescriptorSetRegistry() = default;

		Ref<DescriptorSetManager> GetGlobalSet(const VulkanPipeline &pipeline, uint32_t frame);

		Ref<DescriptorSetManager> GetMaterialSet(const VulkanPipeline &pipeline, VulkanBackendMaterial *mat, uint32_t frame);

		Ref<DescriptorSetManager> GetObjectSet(const VulkanPipeline &pipeline, uint32_t setIndex, uint32_t frame);

		template<typename TKey>
		struct DescriptorSetProvider
		{
			TKey Key;
			Ref<DescriptorSetManager> Manager;
		};

		template <typename TKey, typename THash>
		using EntryMap = std::unordered_map<TKey, DescriptorSetProvider<TKey>, THash>;


		struct GlobalKey
		{
			const VulkanPipeline *Pipeline = nullptr;

			bool operator==(const GlobalKey &other) const { return Pipeline == other.Pipeline; }
		};

		struct MaterialKey
		{
			VulkanBackendMaterial *Mat = nullptr;
			const VulkanPipeline *Pipeline = nullptr;

			bool operator==(const MaterialKey &other) const { return Mat == other.Mat && Pipeline == other.Pipeline; }
		};

		struct ObjectKey
		{
			const VulkanPipeline *Pipeline = nullptr;
			uint32_t SetIndex{};
			bool operator==(const ObjectKey &other) const { return Pipeline == other.Pipeline && SetIndex == other.SetIndex; }
		};

		struct GlobalKeyHash
		{
			size_t operator()(const GlobalKey &k) const { return (std::hash<const void *>()(k.Pipeline) << 1); }
		};

		struct MaterialKeyHash
		{
			size_t operator()(const MaterialKey &k) const { return std::hash<void *>()(k.Mat) ^ (std::hash<const void *>()(k.Pipeline) << 1); }
		};

		struct ObjectKeyHash
		{
			size_t operator()(const ObjectKey &k) const { return std::hash<uint32_t>()(k.SetIndex) ^ (std::hash<const void *>()(k.Pipeline) << 1); }
		};
	
	private:
		EntryMap<GlobalKey, GlobalKeyHash> mGlobalSets;
		EntryMap<MaterialKey, MaterialKeyHash> mMaterialSets;
		EntryMap<ObjectKey, ObjectKeyHash> mObjectSets;
	};

	

} // namespace BHive