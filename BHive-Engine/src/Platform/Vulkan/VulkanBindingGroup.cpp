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

	VulkanBindingGroup::VulkanBindingGroup(VulkanShader *shader, uint32_t setIndex)
		: mSetIndex(setIndex)
	{

		BuildBindings(shader->GetRefl());
		CreateDescriptorSet(shader->GetDescriptorSetLayout(setIndex));
	}

	void VulkanBindingGroup::SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		if (auto info = FindBinding(binding); info->Buffer != buffer)
		{
			info->Buffer = buffer;
			MakeDirty();
		}
	}

	void VulkanBindingGroup::SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		if (auto info = FindBinding(binding); info->Texture != texture || info->MipLevel != mip)
		{
			info->Texture = texture;
			info->MipLevel = mip;
			MakeDirty();
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
			mBindings.push_back(info);
		}
	}

	vk::DescriptorSet VulkanBindingGroup::Update(uint32_t frame)
	{
		if (mNeedsUpdate)
		{
			BuildWriteCopies();

			vk::Device device = VulkanBackend::GetLogicalDevice();
			if (!mCachedWrites.empty())
				device.updateDescriptorSets(mCachedWrites, {});

			mNeedsUpdate = false;
		}

		return mSet;
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

	void VulkanBindingGroup::MakeDirty()
	{
		mNeedsUpdate = true;
	}

	void VulkanBindingGroup::BuildWriteCopies()
	{
		mCachedWrites.clear();
		mCachedImageInfos.clear();
		mCachedBufferInfos.clear();

		auto size = mBindings.size();

		mCachedWrites.reserve(size);
		mCachedImageInfos.reserve(size);
		mCachedBufferInfos.reserve(size);

		for (auto &b : mBindings)
		{
			if (IsBuffer(b.Type))
			{
				if (!b.Buffer)
					continue;

				mCachedBufferInfos.push_back(BuildBufferInfo(b));
				auto &bufInfo = mCachedBufferInfos.back();

				mCachedWrites.emplace_back(mSet, b.Binding, 0, ToVkType(b.Type), nullptr, bufInfo);
			}
			else if (IsTexture(b.Type))
			{
				if (!b.Texture)
					continue;

				mCachedImageInfos.push_back(BuildImageInfo(b, b.MipLevel));
				auto &imgInfo = mCachedImageInfos.back();

				mCachedWrites.emplace_back(mSet, b.Binding, 0, ToVkType(b.Type), imgInfo);
			}
		}
	}

	void VulkanBindingGroup::CreateDescriptorSet(vk::DescriptorSetLayout layout)
	{
		auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
		auto &pools = api->GetDescriptorPoolManager();
		auto pool = pools.GetPool();
		vk::Device device = VulkanBackend::GetLogicalDevice();

		vk::DescriptorSetAllocateInfo allocInfo(pool, {layout});

		mSet = device.allocateDescriptorSets(allocInfo).front();
	}

} // namespace BHive