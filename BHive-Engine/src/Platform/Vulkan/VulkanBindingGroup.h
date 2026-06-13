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

	enum class EBindingUpdateRate
	{
		Static,
		PerFrame,
		PerPass
	};

	struct FBindingInfo
	{
		uint32_t Binding = 0;
		EResourceType Type{};
		EResourceCategory Category{};
		EBindingUpdateRate UpdateRate{};

		Ref<BufferBase> Buffer;
		Ref<Texture> Texture;
		uint32_t MipLevel = 0;
	};

	class VulkanBindingGroup : public IBindingGroup
	{
	public:
		VulkanBindingGroup(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflectionLookUp& refl);

		~VulkanBindingGroup()  = default;

		void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer);

		void SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0);

		void SetTextureImmediate(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0);

		void Update(uint32_t frame) ;

		NativeHandle GetNativeSet(uint32_t frame) ;

		void SetDebugName(const std::string &name);

		const std::vector<FBindingInfo> &GetBindings() const { return mBindings; }

		uint32_t GetSetIndex() const { return mSetIndex; }

	private:
		void BuildBindings(const FShaderReflectionLookUp &refl);

		static EBindingUpdateRate InferUpdateRate(EResourceType type, uint32_t setIndex);

		void AllocateSets();

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &b) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo& bindInfo, uint32_t mip) const;

		FBindingInfo *FindBinding(uint32_t binding);

	private:
		vk::raii::Device& mDevice;

		vk::DescriptorPool mPool;

		vk::DescriptorSetLayout mLayout;
		
		vk::raii::DescriptorSets mSets = VK_NULL_HANDLE;

		uint32_t mSetIndex;

		std::vector<FBindingInfo> mBindings;
	};

	
}