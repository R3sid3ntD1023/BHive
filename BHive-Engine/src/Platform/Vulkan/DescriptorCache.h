#pragma once

#include "VulkanBindingGroup.h"
#include "VulkanCore.h"
#include "core/Core.h"

namespace BHive
{

	struct DescriptorKey
	{
		uint64_t Hash;

		bool operator==(const DescriptorKey &other) const { return Hash == other.Hash; }
	};

	struct MaterialSnapshot;
	struct FPhase;
	struct BindingSetTemplate;

	struct DescriptorBuildInfo
	{
		const BindingSetTemplate *Set;

		const MaterialSnapshot *Snapshot = nullptr;

		vk::DescriptorSetLayout Layout = nullptr;

		const FPhase *Phase = nullptr;
	};

	class DescriptorCache
	{
	public:
		struct CachedDescriptorSet
		{
			VulkanBindingGroup BindingGroup;

			uint32_t LastFrameUsed = 0;
		};

	public:
		DescriptorCache() = default;

		void UpdateCurrentFrame(uint32_t frame) { mCurrentFrame = frame; }

		CachedDescriptorSet &GetOrCreateDescriptorSet(const DescriptorBuildInfo &buildInfo);

	private:
		CachedDescriptorSet &CreateDescriptorSet(const DescriptorKey &key, const DescriptorBuildInfo &buildInfo);

		DescriptorKey CreateDescriptorKey(const DescriptorBuildInfo &buildInfo) const;

		uint64_t ResolveResourceHash(uint32_t set, const DescriptorBuildInfo &buildInfo) const;

	private:
		std::unordered_map<uint64_t, CachedDescriptorSet> mCache;

		uint32_t mCurrentFrame = 0;
	};

} // namespace BHive