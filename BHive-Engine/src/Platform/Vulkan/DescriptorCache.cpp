#include "DescriptorCache.h"
#include "VulkanBackend.h"
#include "VulkanShader.h"
#include "core/utils/Hash.h"
#include "gfx/material/MaterialSnapshot.h"
#include "gfx/rendergraph/Phase.h"
#include "gfx/shader/ShaderTemplate.h"

namespace BHive
{
	DescriptorCache::CachedDescriptorSet &DescriptorCache::GetOrCreateDescriptorSet(const DescriptorBuildInfo &buildInfo)
	{
		auto key = GetDescriptorKey(buildInfo);
		auto it = mCache.find(key);
		if (it != mCache.end())
		{
			return it->second;
		}

		auto &cache = mCache[key];
		cache.ResourceSet = CreateScope<VulkanResourceSet>(*buildInfo.Set);
		UpdateResourceSet(cache.ResourceSet.get(), buildInfo.Snapshot);
		return cache;
	}

	void DescriptorCache::UpdateResourceSet(VulkanResourceSet *set, const MaterialSnapshot *snapShot)
	{
		for (auto &[binding, texture] : snapShot->Textures)
		{
			set->SetTexture(binding, texture.Texture, texture.BaseMipLevel);
		}

		for (auto &[binding, buffer] : snapShot->Buffers)
		{
			set->SetBuffer(binding, buffer.Buffer);
		}
	}

	DescriptorCache::DescriptorKey DescriptorCache::GetDescriptorKey(const DescriptorBuildInfo &buildInfo)
	{
		DescriptorKey key;
		key.LayoutHash = buildInfo.Set->LayoutHash;
		key.ResourceHash = ResolveResourceHash(buildInfo.Snapshot);
		return key;
	}

	uint64_t DescriptorCache::ResolveResourceHash(const MaterialSnapshot *snapShot) const
	{
		uint64_t hash = 0;

		for (auto &[binding, texture] : snapShot->Textures)
		{
			utils::HashCombine(hash, binding, texture.Texture, texture.BaseMipLevel, texture.BaseArrayLayer);
		}

		for (auto &[binding, buffer] : snapShot->Buffers)
		{
			utils::HashCombine(hash, binding, buffer.Buffer);
		}

		return hash;
	}
} // namespace BHive