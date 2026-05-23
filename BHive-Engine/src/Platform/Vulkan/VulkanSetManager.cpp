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
		: SetManagerBase(setIndex),
		  mDevice(device),
		  mPool(pool),
		  mLayout(layout)
	{

		BuildBindings(refl);
		AllocateSets();		
	}

	void VulkanSetManager::SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		FBindingInfo *bindingInfo = nullptr;

		for (auto &b : mBindings)
		{
			if (b.Binding == binding && IsBuffer(b.Type))
			{
				b.Buffer = buffer;
				bindingInfo = &b;
				break;
			}
		}

		if (!bindingInfo)
			return;

		RenderCommand::SubmitResourceUpdate(
			[=](auto &ctx)
			{
				auto vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				auto &set = *mSets[vk_ctx.Frame];
				auto info = BuildBufferInfo(*bindingInfo);
				vk::WriteDescriptorSet write(set, bindingInfo->Binding, 0, ToVkType(bindingInfo->Type), nullptr, info);

				mDevice.updateDescriptorSets(write, {});

					
			});
		
	}

	void VulkanSetManager::SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		FBindingInfo *bindingInfo = nullptr;

		for (auto &b : mBindings)
		{
			if (b.Binding != binding)
				continue;

			switch (b.Type)
			{
			case EResourceType::CombinedImageSampler:
			case EResourceType::SeperatedImage:
			case EResourceType::StorageImage:
			case EResourceType::InputAttachment:
				bindingInfo = &b;
				break;
			default:
				break;
			}

			if (bindingInfo)
				break;
		}

		if (!bindingInfo)
			return;

		bindingInfo->Texture = texture;
		bindingInfo->MipLevel = mip;

		RenderCommand::SubmitResourceUpdate(
			[=](auto &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				auto &set = *mSets[vk_ctx.Frame];
				auto info = BuildImageInfo(*bindingInfo);
				vk::WriteDescriptorSet write(set, bindingInfo->Binding, 0, ToVkType(bindingInfo->Type), info);

				auto image = texture->GetNativeHandle().As<GPUImage>();
				mDevice.updateDescriptorSets(write, {});
			});
	}

	void VulkanSetManager::Update(uint32_t frame)
	{
		auto &set = *mSets[frame];

		std::vector<vk::WriteDescriptorSet> writes;

		for (auto &b : mBindings)
		{
			if (b.UpdateRate != EBindingUpdateRate::PerFrame)
				continue;

			if (IsBuffer(b.Type))
			{
				if (!b.Buffer)
					continue;

				auto info = BuildBufferInfo(b);
				writes.emplace_back(set, b.Binding, 0, ToVkType(b.Type), nullptr, info);
			}
			else if (IsTexture(b.Type))
			{
				if (!b.Texture)
					continue;

				auto info = BuildImageInfo(b);
				writes.emplace_back(set, b.Binding, 0, ToVkType(b.Type), info);
			}
		}

		if (!writes.empty())
			mDevice.updateDescriptorSets(writes, {});
	}

	NativeHandle VulkanSetManager::GetNativeSet(uint32_t frame)
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&*mSets[frame]);
	}

	void VulkanSetManager::AllocateSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mLayout);

		vk::DescriptorSetAllocateInfo alloc_info(mPool, layouts);

		mSets.clear();
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		mSets = vk::raii::DescriptorSets(mDevice, alloc_info);
		for (size_t i = 0; i < mSets.size(); i++)
		{
			VulkanBackend::SetObjectName(*mSets[i], std::format("FrameSet{}", i));
		}
	}

	vk::DescriptorBufferInfo VulkanSetManager::BuildBufferInfo(const FBindingInfo &b) const
	{
		ASSERT(b.Buffer)

		auto native = b.Buffer->GetNativeHandle().As<AllocatedBuffer>();
		return vk::DescriptorBufferInfo(native->GetBuffer(), 0, native->Size);
	}

	vk::DescriptorImageInfo VulkanSetManager::BuildImageInfo(const FBindingInfo &b) const
	{
		vk::DescriptorImageInfo info{};
		auto native = b.Texture->GetNativeHandle().As<GPUImage>();
		auto smp = native->GetSampler();
		auto defView = native->GetDefaultView();

		const uint32_t layer = 0;
		const uint32_t face = 0;
		const uint32_t mip = b.MipLevel;

		switch (b.Type)
		{
		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		{
			ASSERT(smp)
			info = vk::DescriptorImageInfo(smp, native->GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}
		case EResourceType::SeperatedSampler:
		{
			ASSERT(smp)
			info = vk::DescriptorImageInfo(smp);
			break;
		}
		case EResourceType::StorageImage:
		{
			const auto& usage = native->Usage;
			ASSERT(usage & vk::ImageUsageFlagBits::eStorage, "Image is not created with storage usage, cannot be used as a storage image resource -> {}", native->DebugName);
			info = vk::DescriptorImageInfo(nullptr, native->GetView(layer, face, mip), vk::ImageLayout::eGeneral);
			break;
		}
		case EResourceType::InputAttachment:
		{
			info = vk::DescriptorImageInfo(nullptr, native->GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}
		default:
			ASSERT(smp)
			info = vk::DescriptorImageInfo(smp, native->GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}

		return info;
	}
} // namespace BHive