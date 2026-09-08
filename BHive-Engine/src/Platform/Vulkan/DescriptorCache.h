#pragma once

#include "VulkanCore.h"
#include "VulkanResourceSet.h"
#include "core/Core.h"

namespace BHive
{

	struct MaterialSnapshot;
	struct FPhase;
	struct BindingSetTemplate;

	struct DescriptorBuildInfo
	{
		const BindingSetTemplate *Set;

		const MaterialSnapshot *Snapshot = nullptr;
	};

	class DescriptorCache
	{
	public:
		struct CachedDescriptorSet
		{
			Scope<VulkanResourceSet> ResourceSet;
		};

		struct DescriptorKey
		{
			uint64_t LayoutHash;
			uint64_t ResourceHash;

			bool operator==(const DescriptorKey &other) const { return LayoutHash == other.LayoutHash && ResourceHash == other.ResourceHash; }

			size_t operator()(const DescriptorKey &key) const { return key.LayoutHash + key.ResourceHash; }
		};

	public:
		DescriptorCache() = default;

		CachedDescriptorSet &GetOrCreateDescriptorSet(const DescriptorBuildInfo &buildInfo);

	private:
		void UpdateResourceSet(VulkanResourceSet *set, const MaterialSnapshot *snapShot);

		DescriptorKey GetDescriptorKey(const DescriptorBuildInfo &buildInfo);

		uint64_t ResolveResourceHash(const MaterialSnapshot *snapShot) const;

	private:
		std::unordered_map<DescriptorKey, CachedDescriptorSet, DescriptorKey> mCache;
	};

} // namespace BHive