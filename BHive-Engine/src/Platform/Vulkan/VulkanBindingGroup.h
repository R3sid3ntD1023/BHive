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
		VulkanBindingGroup(vk::Device device, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflectionLookUp& refl);

		~VulkanBindingGroup();

		void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer);

		void SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0);

		void Update(uint32_t frame) ;

		vk::DescriptorSet GetFrameSet(uint32_t frame) ;

		void SetDebugName(const std::string &name);

		const std::vector<FBindingInfo> &GetBindings() const { return mBindings; }

		uint32_t GetSetIndex() const { return mSetIndex; }

		vk::DescriptorSet GetOrCreateMaterialSet();

	private:
		void BuildBindings(const FShaderReflectionLookUp &refl);

		static EBindingUpdateRate InferUpdateRate(EResourceType type, uint32_t setIndex);

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &b) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo& bindInfo, uint32_t mip) const;

		FBindingInfo *FindBinding(uint32_t binding);

		struct MaterialKey
		{
			std::vector<uint64_t> Resources;
			bool operator==(const MaterialKey &) const = default;
		};

		struct MaterialKeyHash
		{
			size_t operator()(const MaterialKey& k) const noexcept
			{
				size_t h = 0;
				for (auto id : k.Resources)
					h ^= std::hash<uint64_t>()(id) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
				return h;
			}
		};

		MaterialKey BuildMaterialKey() const;

		vk::DescriptorSet AllocateMaterialSets();

		void WriteDescriptorSet(vk::DescriptorSet set);

	private:
		vk::Device mDevice;

		vk::DescriptorSetLayout mLayout;
		
		std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT> mPerFrameSets{VK_NULL_HANDLE};

		uint32_t mSetIndex;

		std::vector<FBindingInfo> mBindings;

		std::unordered_map<MaterialKey, vk::DescriptorSet, MaterialKeyHash> mMaterialCache;

		std::string mDebugName;
	};

	
}