#include "VulkanBindingGroup.h"
#include "VulkanBackend.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConverters.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanBindingGroup::VulkanBindingGroup(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex, const FShaderReflectionLookUp &refl)
		: mDevice(device),
		  mPool(pool),
		  mLayout(layout),
		  mSetIndex(setIndex)
	{

		BuildBindings(refl);
		AllocateSets();		
	}

	void VulkanBindingGroup::SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		if (auto info = FindBinding(binding))
		{
			info->Buffer = buffer;
		}
	}

	void VulkanBindingGroup::SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		if (auto info = FindBinding(binding))
		{
			info->Texture = texture;
			info->MipLevel = mip;
		}

	}

	void VulkanBindingGroup::BuildBindings(const FShaderReflectionLookUp &refl)
	{
		auto &setBindings = refl.GetSetBindings(mSetIndex);
		mBindings.reserve(setBindings.size());

		for (auto &r : setBindings)
		{
			FBindingInfo info{};
			info.Binding = r.binding;
			info.Type = r.kind;
			info.Category = GetCategory(r.kind);
			info.UpdateRate = InferUpdateRate(r.kind, mSetIndex);
			mBindings.push_back(info);

		}
	}

	EBindingUpdateRate VulkanBindingGroup::InferUpdateRate(EResourceType type, uint32_t setIndex)
	{
		if (setIndex == 0)
			return EBindingUpdateRate::PerFrame;

		switch (type)
		{
		case EResourceType::UniformBuffer:
		case EResourceType::StorageBuffer:
		case EResourceType::InputAttachment:
			return EBindingUpdateRate::PerFrame;
		case EResourceType::StorageImage:
			return EBindingUpdateRate::PerPass;
		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		case EResourceType::SeperatedSampler:
			return (setIndex == 0) ? EBindingUpdateRate::PerFrame : EBindingUpdateRate::PerPass;
		default:
			return EBindingUpdateRate::PerFrame;
		}
	}

	void VulkanBindingGroup::Update(uint32_t frame)
	{
		auto &set = *mSets[frame];

		std::vector<vk::WriteDescriptorSet> writes;
		std::vector<vk::DescriptorImageInfo> imageInfos;
		std::vector<vk::DescriptorBufferInfo> bufferInfos;

		imageInfos.reserve(mBindings.size());
		bufferInfos.reserve(mBindings.size());
		writes.reserve(mBindings.size());

		for (auto &b : mBindings)
		{
			if (b.UpdateRate == EBindingUpdateRate::Static)
				continue;

			if (IsBuffer(b.Type))
			{
				if (!b.Buffer)
					continue;

				bufferInfos.push_back(BuildBufferInfo(b));
				writes.emplace_back(set, b.Binding, 0, ToVkType(b.Type), nullptr, bufferInfos.back());

			}
			else if (IsTexture(b.Type))
			{
				if (!b.Texture)
					continue;

				imageInfos.push_back(BuildImageInfo(b, b.MipLevel));
				writes.emplace_back(set, b.Binding, 0, ToVkType(b.Type), imageInfos.back());
			}
		}

		if (!writes.empty())
			mDevice.updateDescriptorSets(writes, {});
	}

	NativeHandle VulkanBindingGroup::GetNativeSet(uint32_t frame)
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&*mSets[frame]);
	}

	void VulkanBindingGroup::SetDebugName(const std::string &name)
	{
		for (uint32_t i = 0; i < mSets.size(); i++)
		{
			auto setName = std::format("{}[{}]", name, i);
			VulkanBackend::SetObjectName((vk::DescriptorSet)mSets.at(i), setName);
		}
	}

	void VulkanBindingGroup::AllocateSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mLayout);

		vk::DescriptorSetAllocateInfo alloc_info(mPool, layouts);

		mSets.clear();
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		mSets = vk::raii::DescriptorSets(mDevice, alloc_info);
	}

	vk::DescriptorBufferInfo VulkanBindingGroup::BuildBufferInfo(const FBindingInfo &b) const
	{
		ASSERT(b.Buffer)

		auto native = b.Buffer->GetNativeHandle().As<AllocatedBuffer>();
		return vk::DescriptorBufferInfo(native->GetBuffer(), 0, native->Size);
	}

	vk::DescriptorImageInfo VulkanBindingGroup::BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const
	{
		ASSERT(bindInfo.Texture)

		const uint32_t layer = 0;
		const uint32_t face = 0;

		const auto img = bindInfo.Texture->GetNativeHandle().As<VulkanImage>();
		const auto &native = img->Native();
		auto smp = native.GetSampler();
		auto defView = native.GetDefaultView();
		auto view = native.GetView(layer, face, mip);


		LOG_INFO(
			"[DescriptorWrite] set={} binding={} type={}image={} view={} usage={} viewType={} layout={}",
			mSetIndex,
			bindInfo.Binding,
			int(bindInfo.Type),
			(uint64_t)(VkImage)native.GetImage(),
			(uint64_t)(VkImageView)view,
			vk::to_string(native.Usage),
			vk::to_string(native.ViewType),
			vk::to_string(bindInfo.Type == EResourceType::StorageImage
					? vk::ImageLayout::eGeneral
					: vk::ImageLayout::eShaderReadOnlyOptimal))
		
		//LOG_TRACE("BuildImageInfo: binding={}, type={}, tex='{}', mip={}", bindInfo.Binding, int(bindInfo.Type), native.DebugName, mip);
		switch (bindInfo.Type)
		{
		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		{
			ASSERT(smp)
			return vk::DescriptorImageInfo(smp, view, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		case EResourceType::SeperatedSampler:
		{
			ASSERT(smp)
			return vk::DescriptorImageInfo(smp);
		}
		case EResourceType::StorageImage:
		{
			const auto& usage = native.Usage;
			ASSERT(usage & vk::ImageUsageFlagBits::eStorage, "Image is not created with storage usage, cannot be used as a storage image resource -> {}", native.DebugName);
			return vk::DescriptorImageInfo(nullptr, view, vk::ImageLayout::eGeneral);
		}
		case EResourceType::InputAttachment:
		{
			return vk::DescriptorImageInfo(nullptr, view, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		default:
			ASSERT(smp)
			return vk::DescriptorImageInfo(smp, view, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
	}

	FBindingInfo *VulkanBindingGroup::FindBinding(uint32_t binding)
	{
		auto it =  std::find_if(mBindings.begin(), mBindings.end(), [binding](const FBindingInfo &b) { return b.Binding == binding; });
		return it != mBindings.end() ?  &(*it) : nullptr;
	}
} // namespace BHive