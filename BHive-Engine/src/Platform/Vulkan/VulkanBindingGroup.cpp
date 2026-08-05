#include "VulkanBindingGroup.h"
#include "VulkanBackend.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConversions.h"
#include "VulkanRendererAPI.h"
#include "gfx/RenderCommand.h"
#include "VulkanBuffers.h"
#include "VulkanShader.h"

namespace BHive
{

	VulkanBindingGroup::VulkanBindingGroup(vk::Device device, VulkanShader *shader, uint32_t setIndex, const FShaderReflectionLookUp &refl)
		: mShader(shader),
		  mSetIndex(setIndex)
	{

		BuildBindings(refl);
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

	vk::raii::DescriptorSet VulkanBindingGroup::Update(uint32_t frame)
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		auto &pools = api->GetDescriptorPoolManager();
		auto pool = pools.GetPool(mSetIndex, frame);
		auto &device = VulkanBackend::GetLogicalDevice();

		auto layout = mShader->GetDescriptorSetLayout(mSetIndex);
		vk::DescriptorSetAllocateInfo allocInfo(pool, 1, &layout);

		auto set = std::move(device.allocateDescriptorSets(allocInfo).front());

		WriteDescriptorSet(set);
		return set;
	}

	vk::DescriptorBufferInfo VulkanBindingGroup::BuildBufferInfo(const FBindingInfo &bindInfo) const
	{
		ASSERT(bindInfo.Buffer)

		auto handle = bindInfo.Buffer->GetNativeHandle().As<VulkanBuffer>();
		auto &buf = handle->GetNative();
		return vk::DescriptorBufferInfo(buf.GetBuffer(), 0, buf.Size);
	}

	vk::DescriptorImageInfo VulkanBindingGroup::BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const
	{
		ASSERT(bindInfo.Texture)

		const uint32_t layer = 0;
		const uint32_t face = 0;

		const auto img = bindInfo.Texture->GetNativeHandle().As<VulkanImage>();
		ASSERT(img);

		const auto &native = img->Native();
		auto smp = native.GetSampler();
		auto defView = native.GetDefaultView();
		auto view = native.GetView(layer, face, mip);

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
			const auto &usage = native.Usage;
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
		auto it = std::find_if(mBindings.begin(), mBindings.end(), [binding](const FBindingInfo &b) { return b.Binding == binding; });
		return it != mBindings.end() ? &(*it) : nullptr;
	}

	void VulkanBindingGroup::WriteDescriptorSet(vk::DescriptorSet set)
	{
		auto &device = VulkanBackend::GetLogicalDevice();

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
		{
			device.updateDescriptorSets(writes, {});
		}
	}
} // namespace BHive