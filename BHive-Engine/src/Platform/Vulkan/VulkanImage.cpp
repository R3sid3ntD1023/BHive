#include "VulkanImage.h"
#include "Platform/Vulkan/ImageViewBuilder.h"
#include "Platform/Vulkan/VulkanBackend.h"
#include "Platform/Vulkan/VulkanUtils.h"

namespace BHive
{
	VulkanImage::VulkanImage(VulkanImage &&other) noexcept
		: OnDestroyed(std::move(other.OnDestroyed)),
		  mOwnedImage(std::move(other.mOwnedImage)),
		  mImage(other.mImage),
		  mSampler(std::move(other.mSampler)),
		  mAllocator(std::exchange(other.mAllocator, nullptr)),
		  mAllocation(other.mAllocation),
		  mViewInfo(std::move(other.mViewInfo)),
		  mViews(std::move(other.mViews)),
		  mStateTracker(std::move(other.mStateTracker)),
		  mLayers(other.mLayers),
		  mLevels(other.mLevels),
		  mExtents(other.mExtents),
		  mImageAspect(other.mImageAspect)
	{
		other.mImage = VK_NULL_HANDLE;
		other.mAllocation = {};
		other.mViewInfo = vk::ImageViewCreateInfo{};
		other.mLayers = 0;
		other.mLevels = 0;
	}

	VulkanImage &VulkanImage::operator=(VulkanImage &&other) noexcept
	{
		if (this == &other)
			return *this;

		Clear();
		OnDestroyed = std::move(other.OnDestroyed);
		mOwnedImage = std::move(other.mOwnedImage);
		mImage = std::exchange(other.mImage, VK_NULL_HANDLE);
		mSampler = std::move(other.mSampler);
		mAllocation = other.mAllocation;
		other.mAllocation = {};
		mAllocator = std::exchange(other.mAllocator, nullptr);
		mViewInfo = std::move(other.mViewInfo);
		other.mViewInfo = vk::ImageViewCreateInfo{};
		mViews = std::move(other.mViews);
		mStateTracker = std::move(other.mStateTracker);
		mLayers = std::exchange(other.mLayers, 0);
		mLevels = std::exchange(other.mLevels, 0);
		mExtents = other.mExtents;
		mImageAspect = other.mImageAspect;
		return *this;
	}

	VulkanImage::~VulkanImage()
	{
		Clear();
	}

	void VulkanImage::Initialize(const vk::ImageCreateInfo &imgInfo, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo)
	{
		Clear();
		mLayers = imgInfo.arrayLayers;
		mLevels = imgInfo.mipLevels;
		mExtents = imgInfo.extent;
		mImageAspect = viewInfo.subresourceRange.aspectMask;

		auto isCube = viewInfo.viewType == vk::ImageViewType::eCube || viewInfo.viewType == vk::ImageViewType::eCubeArray;

		auto &device = VulkanBackend::GetLogicalDevice();
		mAllocator = &VulkanBackend::GetMemoryAllocator();
		mOwnedImage = device.createImage(imgInfo);
		mImage = *mOwnedImage;
		mAllocation = mAllocator->Allocate(mOwnedImage, vk::MemoryPropertyFlagBits::eDeviceLocal);
		mOwnedImage.bindMemory(mAllocation.Memory, mAllocation.Offset);
		mStateTracker.Initialize(mLayers, mLevels, ImageState::Undefined());

		if (imgInfo.usage & vk::ImageUsageFlagBits::eSampled)
			mSampler = device.createSampler(smpInfo);

		auto view = viewInfo;
		view.setImage(*mOwnedImage);

		mViewInfo = view;
	}

	void VulkanImage::Initialize(
		const vk::Image &img, uint32_t layers, uint32_t levels, vk::ImageUsageFlags usage, const vk::ImageViewCreateInfo &viewInfo, const vk::SamplerCreateInfo &smpInfo
	)
	{
		Clear();
		mImageAspect = viewInfo.subresourceRange.aspectMask;

		auto view = viewInfo;
		view.setImage(img);

		auto &device = VulkanBackend::GetLogicalDevice();
		mImage = img;
		mAllocator = nullptr;

		mLayers = layers;
		mLevels = levels;
		mViewInfo = view;

		if (usage & vk::ImageUsageFlagBits::eSampled)
			mSampler = device.createSampler(smpInfo);

		mStateTracker.Initialize(layers, levels, ImageState::Undefined());
	}

	void VulkanImage::Upload(const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange range)
	{
		auto stagingInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto &device = VulkanBackend::GetLogicalDevice();
		auto &allocator = VulkanBackend::GetMemoryAllocator();
		auto staging = device.createBuffer(stagingInfo);
		auto allocation = allocator.Allocate(staging, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		staging.bindMemory(allocation.Memory, allocation.Offset);
		if (auto mapped = allocator.Map(allocation))
			std::memcpy(mapped, data, size);

		{
			SingleTimeCommand cmd{};
			Transition(cmd, ImageState::TransferWrite(), range);
			VulkanUtils::CopyBufferToImage(cmd, *staging, GetImage(), region);
			Transition(cmd, ImageState::ShaderRead(), range);
		}

		allocator.UnMap(allocation);
		staging = VK_NULL_HANDLE;
		allocator.Free(allocation);
	}

	void VulkanImage::Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange range)
	{
		auto image = GetImage();
		ASSERT(range.LayerCount > 0 && range.LevelCount > 0, "Image transition range must not be empty");

		auto firstOldState = mStateTracker.Get(range.BaseArrayLayer, range.BaseMipLevel);
		bool uniformState = true;
		for (uint32_t layer = range.BaseArrayLayer; layer < range.BaseArrayLayer + range.LayerCount; layer++)
		{
			for (uint32_t mip = range.BaseMipLevel; mip < range.BaseMipLevel + range.LevelCount; mip++)
			{
				const auto &oldState = mStateTracker.Get(layer, mip);
				if (oldState.Layout != firstOldState.Layout || oldState.Access != firstOldState.Access || oldState.Stage != firstOldState.Stage
					|| oldState.IsUndefined != firstOldState.IsUndefined)
					uniformState = false;
			}
		}

		auto transition = [&](const ImageState &oldState, ImageSubresourceRange subresourceRange)
		{
			auto oldLayout = oldState.IsUndefined ? vk::ImageLayout::eUndefined : oldState.Layout;
			auto oldAccess = oldState.IsUndefined ? vk::AccessFlags2{} : oldState.Access;
			auto oldStage = oldState.IsUndefined ? vk::PipelineStageFlags2{} : oldState.Stage;
			if (oldLayout != newState.Layout || oldAccess != newState.Access || oldStage != newState.Stage)
				VulkanUtils::TransitionImageLayout(cmd, image, oldLayout, newState.Layout, oldAccess, newState.Access, oldStage, newState.Stage, mImageAspect, subresourceRange);
		};

		if (uniformState)
		{
			transition(firstOldState, range);
			for (uint32_t layer = range.BaseArrayLayer; layer < range.BaseArrayLayer + range.LayerCount; layer++)
			{
				for (uint32_t mip = range.BaseMipLevel; mip < range.BaseMipLevel + range.LevelCount; mip++)
				{
					auto &state = mStateTracker.Get(layer, mip);
					state = newState;
					state.IsUndefined = false;
				}
			}
			return;
		}

		for (uint32_t layer = range.BaseArrayLayer; layer < range.BaseArrayLayer + range.LayerCount; layer++)
		{
			for (uint32_t mip = range.BaseMipLevel; mip < range.BaseMipLevel + range.LevelCount; mip++)
			{
				auto &oldState = mStateTracker.Get(layer, mip);
				transition(oldState, ImageSubresourceRange{mip, 1, layer, 1});
				oldState = newState;
				oldState.IsUndefined = false;
			}
		}
	}

	void VulkanImage::GenerateMipMaps(vk::CommandBuffer cmd)
	{
		auto w = mExtents.width;
		auto h = mExtents.height;
		auto layers = mLayers;
		auto levels = mLevels;

		vk::Image image = GetImage();

		for (uint32_t mip = 1; mip < levels; ++mip)
		{
			{
				ImageSubresourceRange range{mip - 1, 1, 0, layers};
				Transition(cmd, ImageState::TransferRead(), range);
			}

			{
				ImageSubresourceRange range{mip, 1, 0, layers};
				Transition(cmd, ImageState::TransferWrite(), range);
			}

			// Blit mip -1 -> mip
			vk::ImageBlit blit{};
			blit.srcSubresource = {vk::ImageAspectFlagBits::eColor, mip - 1, 0, layers};
			blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.srcOffsets[1] = vk::Offset3D{int32_t(w), int32_t(h), 1};

			blit.dstSubresource = {vk::ImageAspectFlagBits::eColor, mip, 0, layers};
			blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.dstOffsets[1] = vk::Offset3D{int32_t(std::max(w >> 1, 1u)), int32_t(std::max(h >> 1, 1u)), 1};

			cmd.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);

			w = std::max(w >> 1, 1u);
			h = std::max(h >> 1, 1u);
		}

		ImageSubresourceRange range{0, levels, 0, layers};
		Transition(cmd, ImageState::ShaderRead(), range);
	}

	ImageState VulkanImage::GetState(uint32_t mip, uint32_t layer) const
	{
		return mStateTracker.Get(layer, mip);
	}

	vk::Image VulkanImage::GetImage() const
	{
		return mImage;
	}

	vk::ImageView VulkanImage::GetFullView() const
	{
		return *ImageViewBuilder::GetOrCreateFullView(mViews, mViewInfo, mLayers, mLevels);
	}

	vk::ImageView VulkanImage::GetView(uint32_t layer, uint32_t mip) const
	{
		return *ImageViewBuilder::GetOrCreateView(mViews, mViewInfo, mLayers, mLevels, layer, mip);
	}

	vk::Sampler VulkanImage::GetSampler() const
	{
		return *mSampler;
	}

	void VulkanImage::SetDebugName(const std::string &dbgName)
	{
		VulkanBackend::SetObjectName(GetImage(), dbgName);
	}

	void VulkanImage::Clear()
	{
		if (mImage != VK_NULL_HANDLE && OnDestroyed)
			OnDestroyed();

		mViews.Views.clear();
		mViews.FullView = VK_NULL_HANDLE;
		mViewInfo = vk::ImageViewCreateInfo{};
		mSampler = VK_NULL_HANDLE;
		mOwnedImage = VK_NULL_HANDLE;
		mImage = VK_NULL_HANDLE;
		if (mAllocator)
			mAllocator->Free(mAllocation);
		mAllocator = nullptr;
		mAllocation = {};
	}

} // namespace BHive