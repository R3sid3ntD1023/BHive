#include "VulkanSetManager.h"
#include "VulkanBackend.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConverters.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "textures/VulkanImage.h"

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
		BindingInfo *bindingInfo = nullptr;

		for (auto &b : mBindings)
		{
			if (b.ReflResource.binding == binding && IsBuffer(b.ReflResource.kind))
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
				vk::WriteDescriptorSet write(set, bindingInfo->ReflResource.binding, 0, ToVkType(bindingInfo->ReflResource.kind), nullptr, info);

				mDevice.updateDescriptorSets(write, {});

					
			});
		
	}

	void VulkanSetManager::SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		BindingInfo *bindingInfo = nullptr;

		for (auto &b : mBindings)
		{
			if (b.ReflResource.binding != binding)
				continue;

			switch (b.ReflResource.kind)
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
				vk::WriteDescriptorSet write(set, bindingInfo->ReflResource.binding, 0, ToVkType(bindingInfo->ReflResource.kind), info);

				auto image = texture->GetNativeHandle().As<GPUImage>();
				//LOG_ERROR("Binding texture '{}' to binding {} → actual Vulkan image = {}", bindingInfo->ReflResource.name, binding, image->GetDebugName());
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

			if (IsBuffer(b.ReflResource.kind))
			{
				if (!b.Buffer)
					continue;

				auto info = BuildBufferInfo(b);
				writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), nullptr, info);
			}
			else if (IsTexture(b.ReflResource.kind))
			{
				if (!b.Texture)
					continue;

				auto info = BuildImageInfo(b);
				writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), info);
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

		mSets = vk::raii::DescriptorSets(mDevice, alloc_info);
		for (size_t i = 0; i < mSets.size(); i++)
		{
			VulkanBackend::SetObjectName(*mSets[i], std::format("FrameSet{}", i));
		}
	}

	void VulkanSetManager::WriteStaticBindings()
	{
		RenderCommand::SubmitResourceUpdate(
			[&](auto &ctx)
			{
				auto& vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				std::vector<vk::WriteDescriptorSet> writes;

				for (auto &b : mBindings)
				{
					if (b.UpdateRate != EBindingUpdateRate::Static || (!b.Texture && !b.Buffer))
						continue;

					for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++)
					{
						auto &set = *mSets[frame];

						if (IsBuffer(b.ReflResource.kind))
						{
							auto info = BuildBufferInfo(b);
							writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), nullptr, info);
						}
						else if (IsTexture(b.ReflResource.kind))
						{
							auto info = BuildImageInfo(b);
							writes.emplace_back(set, b.ReflResource.binding, 0, ToVkType(b.ReflResource.kind), info);
						}
					}
				}

				if (!writes.empty())
					mDevice.updateDescriptorSets(writes, {});
			});
	}

	vk::DescriptorBufferInfo VulkanSetManager::BuildBufferInfo(const BindingInfo &b) const
	{
		ASSERT(b.Buffer)

		auto native = b.Buffer->GetNativeHandle().As<AllocatedBuffer>();
		return vk::DescriptorBufferInfo(native->GetBuffer(), 0, native->Size);
	}

	vk::DescriptorImageInfo VulkanSetManager::BuildImageInfo(const BindingInfo &b) const
	{
		vk::DescriptorImageInfo info{};
		auto native = b.Texture->GetNativeHandle().As<GPUImage>();
		auto smp = native->GetSampler();
		auto defView = native->GetDefaultView();

		const uint32_t layer = 0;
		const uint32_t face = 0;
		const uint32_t mip = b.MipLevel;

		switch (b.ReflResource.kind)
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