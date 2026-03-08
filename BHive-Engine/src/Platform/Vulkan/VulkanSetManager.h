#pragma once

#include "VulkanCore.h"
#include "gfx/ISetManager.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class VulkanSetManager : public ISetManager
	{
	public:
		enum class EBindingUpdateRate
		{
			Static,
			PerFrame
		};

		struct BindingInfo
		{
			FReflectedResource ReflResource;
			EBindingUpdateRate UpdateRate = EBindingUpdateRate::Static;

			Ref<BufferBase> Buffer;
			Ref<Texture> Texture;
		};
	public:
		VulkanSetManager(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflectionLookUp& refl);

		~VulkanSetManager() override = default;

		virtual void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer) override;

		virtual void SetTexture(uint32_t binding, const Ref<Texture> &texture) override;

		virtual void Update(uint32_t frame) override;

		virtual NativeHandle GetNativeSet(uint32_t frame) override;

		virtual void WriteStaticBindings() override;


	private:
		void BuildBindings(const FShaderReflectionLookUp &refl);

		void AllocatePerFrameSets();

	
		vk::DescriptorBufferInfo BuildBufferInfo(const BindingInfo &b) const;

		vk::DescriptorImageInfo BuildImageInfo(const BindingInfo &b) const;

	private:
		vk::raii::Device& mDevice;

		vk::DescriptorPool mPool;

		vk::DescriptorSetLayout mLayout;
		
		uint32_t mSetIndex = 0;

		std::vector<BindingInfo> mBindings;

		std::vector<vk::raii::DescriptorSet> mSets;

		
	};

	
}