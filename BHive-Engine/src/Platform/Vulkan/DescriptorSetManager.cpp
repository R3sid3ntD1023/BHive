#include "DescriptorSetManager.h"
#include "VulkanBackend.h"
#include "gfx/BufferBase.h"
#include "gfx/Texture.h"
#include "VulkanConverters.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "textures/VulkanImage.h"

namespace BHive
{

	DescriptorSetManager::DescriptorSetManager(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex, const FShaderReflectionLookUp &refl)
		: mDevice(device),
		  mPool(pool),
		  mLayout(layout),
		  mSetIndex(setIndex)
	{

		BuildBindings(refl);
		AllocateSets();		
	}

	void DescriptorSetManager::SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer)
	{
		FBindingInfo *bindingInfo = FindBinding(binding);

		if (!bindingInfo)
			return;

		bindingInfo->Buffer = buffer;

		FBindingInfo local = *bindingInfo;

		RenderCommand::SubmitResourceUpdate(
			[=](auto &ctx)
			{
				auto& vk_ctx = ctx.As<FVulkanRendererContext>();

				auto &set = *mSets[vk_ctx.Frame];
				auto info = BuildBufferInfo(local);
				vk::WriteDescriptorSet write(set, local.Binding, 0, ToVkType(local.Type), nullptr, info);
				mDevice.updateDescriptorSets(write, {});
			});
		
	}

	void DescriptorSetManager::SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		FBindingInfo* bindingInfo = FindBinding(binding);

		if (!bindingInfo)
			return;

		FBindingInfo local = *bindingInfo;
		local.Texture = texture;
		local.MipLevel = mip;
		local.Binding = binding;

		RenderCommand::SubmitCommand(
			"Bind output mip",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanRendererContext>();

				auto &set = *mSets[vk_ctx.Frame];
				auto imageInfo = BuildImageInfo(local, mip);
				vk::WriteDescriptorSet write(set, local.Binding, 0, ToVkType(local.Type), imageInfo);
				mDevice.updateDescriptorSets(write, {});
			});
	}

	void DescriptorSetManager::SetTextureImmediate(uint32_t binding, const Ref<Texture> &texture, uint32_t mip)
	{
		FBindingInfo *bindingInfo = FindBinding(binding);

		if (!bindingInfo)
			return;

		FBindingInfo local = *bindingInfo;
		local.Texture = texture;
		local.MipLevel = mip;
		local.Binding = binding;

		
		auto &set = *mSets[0];
		auto imageInfo = BuildImageInfo(local, mip);
		vk::WriteDescriptorSet write(set, local.Binding, 0, ToVkType(local.Type), imageInfo);
		mDevice.updateDescriptorSets(write, {});
	}

	void DescriptorSetManager::BuildBindings(const FShaderReflectionLookUp &refl)
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

	EBindingUpdateRate DescriptorSetManager::InferUpdateRate(EResourceType type, uint32_t setIndex)
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
			return EBindingUpdateRate::Static;
		default:
			return EBindingUpdateRate::PerFrame;
		}
	}

	void DescriptorSetManager::Update(uint32_t frame)
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

				auto info = BuildImageInfo(b, b.MipLevel);
				writes.emplace_back(set, b.Binding, 0, ToVkType(b.Type), info);
			}
		}

		if (!writes.empty())
			mDevice.updateDescriptorSets(writes, {});
	}

	NativeHandle DescriptorSetManager::GetNativeSet(uint32_t frame)
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&*mSets[frame]);
	}

	void DescriptorSetManager::SetDebugName(const std::string &name)
	{
		for (uint32_t i = 0; i < mSets.size(); i++)
		{
			auto setName = std::format("{}[{}]", name, i);
			VulkanBackend::SetObjectName((vk::DescriptorSet)mSets.at(i), setName);
		}
	}

	void DescriptorSetManager::AllocateSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mLayout);

		vk::DescriptorSetAllocateInfo alloc_info(mPool, layouts);

		mSets.clear();
		mSets.reserve(MAX_FRAMES_IN_FLIGHT);

		mSets = vk::raii::DescriptorSets(mDevice, alloc_info);
	}

	vk::DescriptorBufferInfo DescriptorSetManager::BuildBufferInfo(const FBindingInfo &b) const
	{
		ASSERT(b.Buffer)

		auto native = b.Buffer->GetNativeHandle().As<AllocatedBuffer>();
		return vk::DescriptorBufferInfo(native->GetBuffer(), 0, native->Size);
	}

	vk::DescriptorImageInfo DescriptorSetManager::BuildImageInfo(const FBindingInfo &bindInfo, uint32_t mip) const
	{
		ASSERT(bindInfo.Texture)
		
		vk::DescriptorImageInfo info{};
		const auto img = bindInfo.Texture->GetNativeHandle().As<VulkanImage>();
		const auto& native = img->Native(); 

		auto smp = native.GetSampler();
		auto defView = native.GetDefaultView();

		const uint32_t layer = 0;
		const uint32_t face = 0;

		//LOG_TRACE("BuildImageInfo: binding={}, type={}, tex='{}', mip={}", bindInfo.Binding, int(bindInfo.Type), native.DebugName, mip);

		switch (bindInfo.Type)
		{
		case EResourceType::CombinedImageSampler:
		case EResourceType::SeperatedImage:
		{
			ASSERT(smp)
			info = vk::DescriptorImageInfo(smp, native.GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
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
			const auto& usage = native.Usage;
			ASSERT(usage & vk::ImageUsageFlagBits::eStorage, "Image is not created with storage usage, cannot be used as a storage image resource -> {}", native.DebugName);
			info = vk::DescriptorImageInfo(nullptr, native.GetView(layer, face, mip), vk::ImageLayout::eGeneral);
			break;
		}
		case EResourceType::InputAttachment:
		{
			info = vk::DescriptorImageInfo(nullptr, native.GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}
		default:
			ASSERT(smp)
			info = vk::DescriptorImageInfo(smp, native.GetView(layer, face, mip), vk::ImageLayout::eShaderReadOnlyOptimal);
			break;
		}

		return info;
	}

	FBindingInfo *DescriptorSetManager::FindBinding(uint32_t binding)
	{
		auto it =  std::find_if(mBindings.begin(), mBindings.end(), [binding](const FBindingInfo &b) { return b.Binding == binding; });
		return it != mBindings.end() ?  &(*it) : nullptr;
	}
} // namespace BHive