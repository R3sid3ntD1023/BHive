#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/NativeHandle.h"
#include "gfx/IBindingGroup.h"
#include "DescriptorPoolManager.h"

namespace BHive
{
	class BufferBase;
	class Texture;
	class VulkanShader;

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
		VulkanBindingGroup(vk::Device device, VulkanShader *shader, uint32_t setIndex, const FShaderReflectionLookUp &refl);

		void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer);

		void SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0);

		vk::raii::DescriptorSet Update(uint32_t frame);

		const std::vector<FBindingInfo> &GetBindings() const { return mBindings; }

		uint32_t GetSetIndex() const { return mSetIndex; }

	private:
		void WriteDescriptorSet(vk::DescriptorSet set);

		void BuildBindings(const FShaderReflectionLookUp &refl);

		static EBindingUpdateRate InferUpdateRate(EResourceType type, uint32_t setIndex);

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &bindInfo) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const;

		FBindingInfo *FindBinding(uint32_t binding);

	private:
		VulkanShader *mShader = nullptr;

		uint32_t mSetIndex;

		std::vector<FBindingInfo> mBindings;
	};

} // namespace BHive