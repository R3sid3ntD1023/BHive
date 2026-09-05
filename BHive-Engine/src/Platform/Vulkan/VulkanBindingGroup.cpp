#include "VulkanBindingGroup.h"
#include "VulkanBackend.h"
#include "VulkanBuffers.h"
#include "VulkanConversions.h"
#include "VulkanRendererAPI.h"
#include "VulkanShader.h"
#include "gfx/BufferBase.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"

namespace BHive
{

	VulkanBindingGroup::VulkanBindingGroup(vk::DescriptorSetLayout layout, const BindingSetTemplate &setTemplate)
		: mSetIndex(setTemplate.SetIndex)
	{
		CreateDescriptorSet(layout);
		Build(setTemplate);
		Build(setTemplate);
	}

	void VulkanBindingGroup::SetBuffer(uint32_t binding, BufferPtr buffer)
	{
		if (auto info = FindBinding(binding); info && info->Buffer != buffer)
		{
			info->Buffer = buffer;
			MakeDirty(binding);
			MakeDirty(binding);
		}
	}

	void VulkanBindingGroup::SetTexture(uint32_t binding, TexturePtr texture, uint32_t mip)
	{
		auto info = FindBinding(binding);
		if (info && (info->Texture != texture || info->MipLevel != mip))
		{
			info->Texture = texture;
			info->MipLevel = mip;

			auto &cachedBinding = mCachedBindings.at(binding);
			cachedBinding.ImageInfo = BuildImageInfo(*info, mip);

			MakeDirty(binding);

			auto &cachedBinding = mCachedBindings.at(binding);
			cachedBinding.ImageInfo = BuildImageInfo(*info, mip);

			MakeDirty(binding);
		}
	}

	vk::DescriptorSet VulkanBindingGroup::Update(uint32_t frame)
	{
		std::vector<vk::WriteDescriptorSet> writes;
		for (auto &binding : mDirtyBindings)
		{
			auto &info = mBindings.at(binding);
			CachedWrite &write = mCachedBindings.at(binding).Writes[frame];

			if (IsBuffer(info.Type))
				write.BufferInfo = BuildBufferInfo(info, frame);

			writes.emplace_back(write.Write);
		}

		if (!writes.empty())
		{
			auto &device = VulkanBackend::GetLogicalDevice();
			device.updateDescriptorSets(writes, {});
			std::vector<vk::WriteDescriptorSet> writes;
			for (auto &binding : mDirtyBindings)
			{
				auto &info = mBindings.at(binding);
				CachedWrite &write = mCachedBindings.at(binding).Writes[frame];

				if (IsBuffer(info.Type))
					write.BufferInfo = BuildBufferInfo(info, frame);

				writes.emplace_back(write.Write);
			}

			if (!writes.empty())
			{
				auto &device = VulkanBackend::GetLogicalDevice();
				device.updateDescriptorSets(writes, {});
			}

			return mSets[frame];
		}

		vk::DescriptorBufferInfo VulkanBindingGroup::BuildBufferInfo(const FBindingInfo &bindInfo, uint32_t frame) const
		{
			ASSERT(bindInfo.Buffer)

			auto handle = bindInfo.Buffer.As<BufferBase>()->GetNativeHandle().As<VulkanBuffer>();
			auto buf = handle->GetNative(frame);
			return vk::DescriptorBufferInfo(buf->Buffer, 0, buf->Size);
		}

		vk::DescriptorImageInfo VulkanBindingGroup::BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const
		{
			ASSERT(bindInfo.Texture)

			const uint32_t layer = 0;
			const uint32_t face = 0;

			const auto img = bindInfo.Texture.As<Texture>()->GetNativeHandle().As<VulkanImage>();
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
			if (!mBindings.contains(binding))
				return nullptr;
			return &mBindings.at(binding);
			if (!mBindings.contains(binding))
				return nullptr;
			return &mBindings.at(binding);
		}

		void VulkanBindingGroup::MakeDirty(uint32_t binding) void VulkanBindingGroup::MakeDirty(uint32_t binding)
		{
			mDirtyBindings.insert(binding);
			mDirtyBindings.insert(binding);
		}

		void VulkanBindingGroup::Build(const BindingSetTemplate &setTemplate) void VulkanBindingGroup::Build(const BindingSetTemplate &setTemplate)
		{
			auto &setBindings = setTemplate.Bindings;
			mBindings.reserve(setBindings.size());

			for (auto &b : setBindings)
			{
				// store binding info
				mBindings[b.Binding] = {b.Type, GetCategory(b.Type)};

				auto &cachedBinding = mCachedBindings[b.Binding];

				for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				{
					auto &write = cachedBinding.Writes[i];
					write.Write.dstSet = mSets[i];
					write.Write.dstArrayElement = 0;
					write.Write.dstBinding = b.Binding;
					write.Write.descriptorType = ToVkType(b.Type);
					write.Write.descriptorCount = b.ArraySize;

					if (IsBuffer(b.Type))
					{
						write.Write.pBufferInfo = &write.BufferInfo;
					}
					else
					{
						write.Write.pImageInfo = &cachedBinding.ImageInfo;
					}
					if (IsBuffer(b.Type))
					{
						write.Write.pBufferInfo = &write.BufferInfo;
					}
					else
					{
						write.Write.pImageInfo = &cachedBinding.ImageInfo;
					}
				}
			}
		}

		void VulkanBindingGroup::CreateDescriptorSet(vk::DescriptorSetLayout layout)
		{
			auto pool = VulkanBackend::GetDescriptorPool();
			vk::Device device = VulkanBackend::GetLogicalDevice();

			std::vector<vk::DescriptorSetLayout> layouts{2, layout};
			vk::DescriptorSetAllocateInfo allocInfo(pool, layouts);

			mSets = device.allocateDescriptorSets(allocInfo);
		}

	} // namespace BHive