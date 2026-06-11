#pragma once

#include "VulkanCore.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/NativeHandle.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	class DescriptorSetManager
	{
	public:
		struct FImageWriteInfo
		{
			uint32_t Binding{};
			uint32_t BaseMipLevel{0};
			Ref<Texture> TetxureRef;
		};

		struct FBufferWriteInfo
		{
			uint32_t Binding{0};
			Ref<BufferBase> BufferRef;
		};

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

	public:
		DescriptorSetManager(vk::raii::Device &device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflectionLookUp& refl);

		void Write(const FBufferWriteInfo& writeInfo, uint32_t frame);

		void Write(const FImageWriteInfo& writeInfo, uint32_t frame);

		void Update(uint32_t frame) ;

		NativeHandle GetNativeSet(uint32_t frame);

	private:

		void BuildBindings(const FShaderReflectionLookUp &refl, uint32_t setIndex);

		static EBindingUpdateRate InferUpdateRate(EResourceType type, uint32_t setIndex);

		void AllocateSets();

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &b) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo& bindInfo) const;

		FBindingInfo *FindBinding(uint32_t binding);

	private:
		vk::raii::Device& mDevice;

		vk::DescriptorPool mPool;

		vk::DescriptorSetLayout mLayout;
		
		vk::raii::DescriptorSets mSets = VK_NULL_HANDLE;

		std::vector<FBindingInfo> mBindings;
	};

	
}