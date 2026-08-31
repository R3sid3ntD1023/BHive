#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/NativeHandle.h"
#include "gfx/IBindingGroup.h"

namespace BHive
{
	class BufferBase;
	class Texture;
	class VulkanShader;

	struct FBindingInfo
	{
		uint32_t Binding = 0;
		EResourceType Type{};
		EResourceCategory Category{};

		Ref<BufferBase> Buffer;
		TexturePtr Texture;
		uint32_t MipLevel = 0;
	};

	class VulkanBindingGroup : public IBindingGroup
	{
	public:
		VulkanBindingGroup(VulkanShader *shader, uint32_t setIndex);

		void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer);

		void SetTexture(uint32_t binding, TexturePtr texture, uint32_t mip = 0);

		vk::DescriptorSet Update(uint32_t frame);

		const std::vector<FBindingInfo> &GetBindings() const { return mBindings; }

		uint32_t GetSetIndex() const { return mSetIndex; }

	private:
		void BuildBindings(const FShaderReflectionLookUp &refl);

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &bindInfo, uint32_t frame) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const;

		FBindingInfo *FindBinding(uint32_t binding);

		void MakeDirty();

		void BuildWriteCopies(uint32_t frame);

		void CreateDescriptorSet(vk::DescriptorSetLayout layout);

	private:
		uint32_t mSetIndex;

		std::vector<FBindingInfo> mBindings;

		std::bitset<2> mNeedsUpdate;
		std::vector<vk::WriteDescriptorSet> mCachedWrites;
		std::vector<vk::DescriptorImageInfo> mCachedImageInfos;
		std::vector<vk::DescriptorBufferInfo> mCachedBufferInfos;
		std::vector<vk::DescriptorSet> mSets;
	};

} // namespace BHive