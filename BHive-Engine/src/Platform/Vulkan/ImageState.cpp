#include "ImageState.h"

namespace BHive
{
	ImageState::ImageState(vk::ImageLayout layout, vk::AccessFlags2 access, vk::PipelineStageFlags2 stage, bool unDefined)
		: Layout(layout),
		  Access(access),
		  Stage(stage),
		  IsUndefined(unDefined)
	{
	}

	ImageState ImageState::Undefined()
	{
		return {vk::ImageLayout::eUndefined, {}, vk::PipelineStageFlagBits2::eTopOfPipe, true};
	}

	ImageState ImageState::Present()
	{
		return {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe, false};
	}

	ImageState ImageState::ColorAttachment()
	{
		return {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput, false};
	}

	ImageState ImageState::DepthStencilAttachment()
	{
		return {
			vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests, false};
	}

	ImageState ImageState::ShaderRead()
	{
		return {
			vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead | vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentRead,
			vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader, false};
	}

	ImageState ImageState::ComputeRead()
	{
		return {vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead, vk::PipelineStageFlagBits2::eComputeShader, false};
	}

	ImageState ImageState::ComputeWrite()
	{
		return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eShaderWrite, vk::PipelineStageFlagBits2::eComputeShader, false};
	}

	ImageState ImageState::TransferRead()
	{
		return {vk::ImageLayout::eTransferSrcOptimal, vk::AccessFlagBits2::eTransferRead, vk::PipelineStageFlagBits2::eTransfer, false};
	}

	ImageState ImageState::TransferWrite()
	{
		return {vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer, false};
	}

	ImageState ImageState::TransferClear()
	{
		return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer, false};
	}

	ImageState ImageState::ToImageState(EImageUsage access)
	{
		switch (access)
		{
		case BHive::EImageUsage::ColorRead:
			return ImageState::ShaderRead();
		case BHive::EImageUsage::ColorWrite:
			return ImageState::ColorAttachment();
		case BHive::EImageUsage::DepthRead:
			return ImageState::ShaderRead();
		case BHive::EImageUsage::DepthWrite:
			return ImageState::DepthStencilAttachment();
		case BHive::EImageUsage::ComputeSampled:
			return ImageState::ShaderRead();
		case BHive::EImageUsage::ComputeStorageRead:
			return ImageState::ComputeRead();
		case BHive::EImageUsage::ComputeStorageWrite:
			return ImageState::ComputeWrite();
		case BHive::EImageUsage::TransferSrc:
			return ImageState::TransferRead();
		case BHive::EImageUsage::TransferDst:
			return ImageState::TransferWrite();
		default:
			return Undefined();
		}
	}
} // namespace BHive
