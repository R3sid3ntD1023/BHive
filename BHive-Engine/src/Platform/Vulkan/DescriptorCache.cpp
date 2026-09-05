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
		auto key = CreateDescriptorKey(buildInfo);
		auto it = mCache.find(key.Hash);
		if (it != mCache.end())
		{
			it->second.LastFrameUsed = mCurrentFrame;
			return it->second;
		}

		// CachedDescriptorSet newSet;
		// newSet.BindingGroup = VulkanBindingGroup(*buildInfo.Set, buildInfo.Layout);
		// newSet.LastFrameUsed = mCurrentFrame;
		// mCache[key.Hash] = newSet;
		return mCache.at(key.Hash);
	}

	DescriptorKey DescriptorCache::CreateDescriptorKey(const DescriptorBuildInfo &buildInfo) const
	{
		DescriptorKey key{};
		auto resource = ResolveResourceHash(buildInfo.Set->SetIndex, buildInfo);
		utils::HashCombine(key.Hash, buildInfo.Set->LayoutHash, resource);
		return key;
	}

	uint64_t DescriptorCache::ResolveResourceHash(uint32_t set, const DescriptorBuildInfo &buildInfo) const
	{
		const auto *setTemplate = buildInfo.Set;
		auto *snapShot = buildInfo.Snapshot;
		auto *phase = buildInfo.Phase;

		uint64_t hash = 0;

		// if (setTemplate->SetIndex == MATERIAL_SET_INDEX)
		// {
		// 	for (auto &[nameHash, texture] : snapShot->Textures)
		// 	{
		// 		utils::HashCombine(hash, texture.Texture, texture.Binding);
		// 	}

		// 	for (auto &[nameHash, buffer] : snapShot->LocalBuffers)
		// 	{
		// 		utils::HashCombine(hash, buffer.Buffer, buffer.Binding);
		// 	}
		// }

		// for (auto &[binding, texture] : phase->BoundTextures)
		// {
		// 	if (binding.Set != setTemplate->SetIndex)
		// 		continue;

		// 	utils::HashCombine(hash, binding.Binding, binding.Set, texture);
		// }

		// for (auto &[binding, buffer] : phase->BoundBuffers)
		// {
		// 	if (binding.Set != setTemplate->SetIndex)
		// 		continue;
		// 	utils::HashCombine(hash, binding.Binding, binding.Set, buffer);
		// }
		return hash;
	}
} // namespace BHive