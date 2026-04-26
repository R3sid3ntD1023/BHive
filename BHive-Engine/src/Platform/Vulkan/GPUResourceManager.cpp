#include "GPUResourceManager.h"
#include "VulkanUtils.h"
#include "VulkanBackend.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "GPUComponents.h"

namespace BHive
{
	AllocatedBuffer GPUResourceManager::CreateBuffer(const BufferDesc &desc)
	{
		AllocatedBuffer out{};

		auto handle = UUID();
		auto &buffer = GetStorage<vk::raii::Buffer>().GetOrCreate(handle);

		VulkanUtils::CreateBuffer(desc.Size, desc.Usage, desc.MemoryFlags, buffer);

		auto& allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(buffer, desc.MemoryFlags, desc.Size);

		buffer.bindMemory(allocation.Memory, allocation.Offset);

		out.Buffer = handle;
		out.Allocation = allocation;
		out.Size = desc.Size;
		return out;
	}

	GPUImage GPUResourceManager::CreateImage(const ImageDesc &desc)
	{
		//LOG_INFO("GPUResourceManager::CreateImage {} usage = 0x{:X}", desc.DebugName, (uint32_t)desc.Usage);

		GPUImage out{};

		auto handle = UUID();
		auto &image = GetStorage<vk::raii::Image>().GetOrCreate(handle);

		VulkanUtils::CreateImage(desc.Flags, desc.MipLevels, desc.Width, desc.Height, desc.Depth, desc.ArrayLayers, desc.Type, desc.Format, desc.Tiling, desc.Usage, desc.MemoryFlags, image);

		auto &allocator = VulkanBackend::GetMemoryAllocator();
		MemoryAllocation allocation = allocator.Allocate(image, desc.MemoryFlags, desc.Size());

		image.bindMemory(allocation.Memory, allocation.Offset);

		out.ImageHandle = handle;
		out.ArrayLayers = desc.ArrayLayers;
		out.Allocation = std::move(allocation);
		out.Aspect = desc.Aspect;
		out.DebugName = desc.DebugName;
		out.Usage = desc.Usage;

		auto* state = out.AddComponent<StateTrackingComponent>();
		state->Init(desc.ArrayLayers, desc.MipLevels, {vk::ImageLayout::eUndefined, vk::AccessFlagBits2::eNone, vk::PipelineStageFlagBits2::eTopOfPipe});

		return out;
	}

	UUID GPUResourceManager::RegisterExternalImage(const vk::Image &image)
	{
		UUID id = UUID();
		auto img = vk::raii::Image(VulkanBackend::GetLogicalDevice(), image);
		GetStorage<vk::raii::Image>().AddExternal(id, std::move(img));
		mExternalImages.insert(id);

		return id;
	}

	UUID GPUResourceManager::CreateImageView(const vk::Image &image, const ImageViewDesc &desc)
	{
		auto handle = UUID();
		auto &view = GetStorage<vk::raii::ImageView>().GetOrCreate(handle);
		VulkanUtils::CreateImageView(image, view, desc.Type, desc.Format, desc.Aspect, desc.BaseMipLevel, desc.LevelCount, desc.BaseArrayLayer, desc.LayerCount);
		return handle;
	}

	void* GPUResourceManager::MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size)
	{
		if (!buffer.Buffer )
			return nullptr;

		auto &allocation = buffer.Allocation;
		if (allocation.IsMapped)
		{
			return static_cast<char *>(allocation.MappedPtr) + offset;
		}

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		return allocator.Map(allocation);
	}

	void GPUResourceManager::UnmapMemory(AllocatedBuffer &buffer)
	{
		if (!buffer.Buffer || !buffer.Allocation.IsMapped || buffer.Allocation.IsDedicated)
			return;

		MemoryAllocator &allocator = VulkanBackend::GetMemoryAllocator();
		allocator.UnMap(buffer.Allocation);
	}

	void GPUResourceManager::CreateSampler(GPUImage &image, const vk::SamplerCreateInfo &create_info)
	{
		auto handle = UUID();
		auto &sampler = GetStorage<vk::raii::Sampler>().GetOrCreate(handle);

		VulkanUtils::CreateImageSampler(sampler, create_info);

		auto *smpComp = image.AddComponent<SamplerComponent>();
		smpComp->Sampler = handle;
	}

	void GPUResourceManager::DestroyBuffer(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, handle](uint32_t) {
				auto &storage = GetStorage<vk::raii::Buffer>();
				storage.Remove(handle);
			});
		
	}

	void GPUResourceManager::DestroyImage(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				if (mExternalImages.contains(handle))
				{
					mExternalImages.erase(handle);
					GetStorage<vk::raii::Image>().Remove(handle);
					return;
				}

				GetStorage<vk::raii::Image>().Remove(handle);
			});
	}

	void GPUResourceManager::DestroyImageView(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::ImageView>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroySampler(const UUID& handle)
	{
		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, handle](uint32_t)
			{
				auto &storage = GetStorage<vk::raii::Sampler>();
				storage.Remove(handle);
			});
	}

	void GPUResourceManager::DestroyBuffer(AllocatedBuffer buffer)
	{
		DestroyBuffer(buffer.Buffer);

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion(
			[this, alloc = buffer.Allocation](uint32_t)
			{
				VulkanBackend::GetMemoryAllocator().Free(alloc);
			});
	}

	void GPUResourceManager::DestroyImage(GPUImage& image)
	{
		if (auto smp = image.GetComponent<SamplerComponent>())
			DestroySampler(smp->Sampler);

		if (auto def = image.GetComponent<DefaultViewComponent>())
			DestroyImageView(def->View);

		if (auto mips = image.GetComponent<MipViewComponent>())
		{
			for (auto& layer : mips->Views)
				for (auto &mip : layer)
					DestroyImageView(mip);
		}

		if (auto cube = image.GetComponent<CubeMipViewComponent>())
		{
			for (auto &perCube : cube->Views)
				for (auto& mip : perCube)
					DestroyImageView(mip);
		}

		if (auto face = image.GetComponent<FaceMipViewComponent>())
		{
			for (auto &faceMips : face->Views)
			{
				for (auto &perFace : faceMips)
					for (auto &mip : perFace)
						DestroyImageView(mip);
			}
		}

		DestroyImage(image.ImageHandle);

		if (image.Allocation.IsDedicated)
			 return;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([this, alloc = image.Allocation](uint32_t) {
			
			VulkanBackend::GetMemoryAllocator().Free(alloc);
		});
	}


	const vk::Image &GPUResourceManager::GetImage(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Image>();
		return *storage.Get(handle);
	}

	const vk::ImageView &GPUResourceManager::GetImageView(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::ImageView>();
		return *storage.Get(handle);
	}

	const vk::Sampler &GPUResourceManager::GetSampler(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Sampler>();
		return *storage.Get(handle);
	}

	const vk::Buffer &GPUResourceManager::GetBuffer(const UUID &handle)
	{
		auto &storage = GetStorage<vk::raii::Buffer>();
		return *storage.Get(handle);
	}

	void GPUResourceManager::Create2DViews(GPUImage &image, const ImageViewDesc &desc)
	{
		auto &vk_image = image.GetImage();
		auto *def = image.AddComponent<DefaultViewComponent>();
		auto *mips = image.AddComponent<MipViewComponent>();

		// default view
		{
			ImageViewDesc default_desc = desc;
			default_desc.BaseMipLevel = 0;
			default_desc.LevelCount = image.MipLevels;
			default_desc.BaseArrayLayer = 0;
			default_desc.LayerCount = image.ArrayLayers;
			def->View = CreateImageView(vk_image, default_desc);
		}

		// per mip 2d views
		mips->Views.resize(image.ArrayLayers);
		for (uint32_t layer = 0; layer < image.ArrayLayers; layer++)
		{
			mips->Views[layer].resize(image.MipLevels);
			for (uint32_t mip = 0; mip < image.MipLevels; mip++)
			{
				ImageViewDesc mip_desc = desc;
				mip_desc.BaseMipLevel = mip;
				mip_desc.LevelCount = 1;
				mip_desc.BaseArrayLayer = 0;
				mip_desc.LayerCount = image.ArrayLayers;
				mips->Views[layer][mip] = CreateImageView(vk_image, mip_desc);
			}
		}
	}

	void GPUResourceManager::CreateCubeViews(GPUImage &image, const ImageViewDesc &desc)
	{
		auto &vk_image = image.GetImage();
		auto *def = image.AddComponent<DefaultViewComponent>();
		auto *cubeMips = image.AddComponent<CubeMipViewComponent>();
		auto *faceMips = image.AddComponent<FaceMipViewComponent>();
		const uint32_t cubeCount = image.ArrayLayers; // number of cubes
		const uint32_t facesPerCube = 6;

		// default view
		{
			ImageViewDesc default_desc = desc;
			default_desc.Type = vk::ImageViewType::eCube;
			default_desc.BaseMipLevel = 0;
			default_desc.LevelCount = image.MipLevels;
			default_desc.BaseArrayLayer = 0;
			default_desc.LayerCount = image.ArrayLayers * 6;
			def->View = CreateImageView(vk_image, default_desc);
		}

		

		// per mip 2d views
		cubeMips->Views.resize(cubeCount);
		for (uint32_t layer = 0; layer < cubeCount; layer++)
		{
			cubeMips->Views[layer].resize(image.MipLevels);
			for (uint32_t mip = 0; mip < image.MipLevels; mip++)
			{
				ImageViewDesc mip_desc = desc;
				mip_desc.Type = vk::ImageViewType::eCube;
				mip_desc.BaseMipLevel = mip;
				mip_desc.LevelCount = 1;
				mip_desc.BaseArrayLayer = 0;
				mip_desc.LayerCount = image.ArrayLayers * 6;
				cubeMips->Views[layer][mip] = CreateImageView(vk_image, mip_desc);
			}
		}
		

		//face+mip 2d views
		faceMips->Views.resize(cubeCount);
		for (uint32_t layer = 0; layer < cubeCount; layer++)
		{
			faceMips->Views[layer].resize(facesPerCube);
			for (uint32_t face = 0; face < facesPerCube; face++)
			{
				faceMips->Views[layer][face].resize(image.MipLevels);
				for (uint32_t mip = 0; mip < image.MipLevels; mip++)
				{
					ImageViewDesc mip_desc = desc;
					mip_desc.Type = vk::ImageViewType::e2D;
					mip_desc.BaseMipLevel = mip;
					mip_desc.LevelCount = 1;
					mip_desc.BaseArrayLayer = face;
					mip_desc.LayerCount = 1;
					faceMips->Views[layer][face][mip] = CreateImageView(vk_image, mip_desc);
				}
			}
		}
		
	}

} // namespace BHive