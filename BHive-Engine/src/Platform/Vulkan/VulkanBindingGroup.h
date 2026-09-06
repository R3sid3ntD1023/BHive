#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/NativeHandle.h"
#include "gfx/ResourceSet.h"
#include "gfx/registries/Handles.h"
#include "gfx/shader/ShaderTemplate.h"

namespace BHive
{
	struct FBindingInfo
	{
		EResourceType Type{};
		EResourceCategory Category{};
		uint32_t MipLevel = 0;

		BufferPtr Buffer;
		TexturePtr Texture;
	};

	class VulkanBindingGroup : public ResourceSet
	{
		struct CachedWrite
		{
			vk::WriteDescriptorSet Write;
			vk::DescriptorBufferInfo BufferInfo;
		};

		struct CachedBinding
		{
			vk::DescriptorImageInfo ImageInfo;
			CachedWrite Writes[MAX_FRAMES_IN_FLIGHT];
		};

	public:
		VulkanBindingGroup() = default;

		VulkanBindingGroup(const BindingSetTemplate &setTemplate);

		void SetBuffer(uint32_t binding, BufferPtr buffer);

		void SetTexture(uint32_t binding, TexturePtr texture, uint32_t mip = 0);

		vk::DescriptorSet Update(uint32_t frame);

		uint32_t GetSetIndex() const { return mSetIndex; }

	private:
		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &bindInfo, uint32_t frame) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const;

		FBindingInfo *FindBinding(uint32_t binding);

		void MakeDirty(uint32_t binding);

		void Build(const BindingSetTemplate &setTemplate);

		void CreateDescriptorSet(vk::DescriptorSetLayout layout);

	private:
		uint32_t mSetIndex;

		std::vector<vk::DescriptorSet> mSets;
		std::unordered_map<uint32_t, CachedBinding> mCachedBindings;
		std::unordered_map<uint32_t, FBindingInfo> mBindings;
		std::unordered_set<uint32_t> mDirtyBindings;
	};

} // namespace BHive