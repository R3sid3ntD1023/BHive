#include "VulkanSetManager.h"
#include "VulkanBackend.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConverters.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanSetManager::VulkanSetManager(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex, const FShaderReflectionLookUp &refl)
		: mDevice(device),
		  mPool(pool),
		  mLayout(layout),
		  mSetIndex(setIndex)
	{

		BuildBindings(refl);
		AllocatePerFrameSets();		
	}

	void VulkanSetManager::SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		for (auto &b : mBindings)
		{
			if (b.ReflResource.binding == binding && IsBuffer(b.ReflResource.kind))
			{
				b.Buffer = buffer;

				std::vector<vk::WriteDescriptorSet> writes;
				for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame)
				{
					auto &set = *mSets[frame];
					auto info = BuildBufferInfo(b);
					writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), nullptr, info);
				}
				if (!writes.empty())
					mDevice.updateDescriptorSets(writes, {});

				break;
			}
		}
	}

	void VulkanSetManager::SetTexture(uint32_t binding, const Ref<Texture> &texture)
	{
		for (auto& b : mBindings)
		{
			if (b.ReflResource.binding == binding && IsTexture(b.ReflResource.kind))
			{
				b.Texture = texture;

				std::vector<vk::WriteDescriptorSet> writes;
				for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++)
				{
					auto &set = *mSets[frame];
					auto info = BuildImageInfo(b);
					writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), info);
				}
				if (!writes.empty())
					mDevice.updateDescriptorSets(writes, {});

				break;
			}
		}
	}

	void VulkanSetManager::Update(uint32_t frame)
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)

		auto& set = *mSets[frame];

		std::vector<vk::WriteDescriptorSet> writes;

		for (auto &b : mBindings)
		{
			if (b.UpdateRate != EBindingUpdateRate::PerFrame)
				continue;

			auto info = BuildBufferInfo(b);

			writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), nullptr, info);
		}

		if (!writes.empty())
			mDevice.updateDescriptorSets(writes, {});
	}

	NativeHandle VulkanSetManager::GetNativeSet(uint32_t frame)
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&*mSets[frame]);
	}

	void VulkanSetManager::BuildBindings(const FShaderReflectionLookUp &refl)
	{
		auto &setBindings = refl.GetSetBindings(mSetIndex);
		mBindings.reserve(setBindings.size());

		for (auto& r : setBindings)
		{
			BindingInfo info{};
			info.ReflResource = r;
			
			switch (r.kind)
			{
			case EResourceType::UniformBuffer:
			case EResourceType::StorageBuffer:
			case EResourceType::StorageImage:
				info.UpdateRate = EBindingUpdateRate::PerFrame;
				break;
			case EResourceType::CombinedImageSampler:
				info.UpdateRate = EBindingUpdateRate::Static;
				break;
			default:
				info.UpdateRate = EBindingUpdateRate::Static;
				break;
			}

			mBindings.push_back(info);
		}
	}

	void VulkanSetManager::AllocatePerFrameSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mLayout);

		vk::DescriptorSetAllocateInfo alloc_info(mPool, layouts);

		mSets.clear();
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		auto sets = mDevice.allocateDescriptorSets(alloc_info);
		for (auto& s : sets)
			mSets.emplace_back(std::move(s));
	}

	void VulkanSetManager::WriteStaticBindings()
	{
		std::vector<vk::WriteDescriptorSet> writes;

		for (auto& b : mBindings)
		{
			if (b.UpdateRate != EBindingUpdateRate::Static)
				continue;

			for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++)
			{
				auto &set = *mSets[frame];
				auto info = BuildImageInfo(b);
				writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), info);
			}
			
		}

		if (!writes.empty())
			mDevice.updateDescriptorSets(writes, {});
	}

	vk::DescriptorBufferInfo VulkanSetManager::BuildBufferInfo(const BindingInfo &b) const
	{
		ASSERT(b.Buffer)

		auto native = b.Buffer->GetNativeHandle().As<AllocatedBuffer>();
		return vk::DescriptorBufferInfo(native->Buffer, 0, native->Size);
	}

	vk::DescriptorImageInfo VulkanSetManager::BuildImageInfo(const BindingInfo &b) const
	{
		ASSERT(b.Texture)

		vk::DescriptorImageInfo info{};
		auto native = b.Texture->GetNativeHandle().As<AllocatedImage>();

		ASSERT(native);
		ASSERT(native->GetView() != VK_NULL_HANDLE);
		ASSERT(native->GetSampler() != VK_NULL_HANDLE || b.ReflResource.kind == EResourceType::SeperatedImage);

		switch (b.ReflResource.kind)
		{
		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		{
			info = vk::DescriptorImageInfo(native->GetSampler(), native->GetView(), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}
		case EResourceType::SeperatedSampler:
		{
			info = vk::DescriptorImageInfo(native->GetSampler());
			break;
		}
		case EResourceType::StorageImage:
		{
			info = vk::DescriptorImageInfo(nullptr, native->GetView(), vk::ImageLayout::eGeneral);
			break;
		}
		case EResourceType::InputAttachment:
		{
			info = vk::DescriptorImageInfo(nullptr, native->GetView(), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}
		default:
			info = vk::DescriptorImageInfo(native->GetSampler(), native->GetView(), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}

		return info;
	}
} // namespace BHive