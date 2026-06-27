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
		return {vk::ImageLayout::eUndefined, {}, vk::PipelineStageFlagBits2::eTopOfPipe};
	}

	ImageState ImageState::Present()
	{
		return {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe};
	}

	ImageState ImageState::ColorAttachment()
	{
		return {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput};
	}

	ImageState ImageState::DepthStencilAttachment()
	{
		return {
			vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests};
	}

	ImageState ImageState::ShaderRead()
	{
		return {
			vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead,
			vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader};
	}

	ImageState ImageState::ComputeRead()
	{
		return {vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead, vk::PipelineStageFlagBits2::eComputeShader};
	}

	ImageState ImageState::ComputeWrite()
	{
		return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eShaderWrite, vk::PipelineStageFlagBits2::eComputeShader};
	}

	ImageState ImageState::TransferRead()
	{
		return {vk::ImageLayout::eTransferSrcOptimal, vk::AccessFlagBits2::eTransferRead, vk::PipelineStageFlagBits2::eTransfer};
	}

	ImageState ImageState::TransferWrite()
	{
		return {vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer};
	}

	ImageState ImageState::TransferClear()
	{
		return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer};
	}

	ImageState ImageState::ToImageState(EImageAccess access)
	{
		switch (access)
		{
		case BHive::EImageAccess::ColorRead:
			return ImageState::ShaderRead();
		case BHive::EImageAccess::ColorWrite:
			return ImageState::ColorAttachment();
		case BHive::EImageAccess::DepthRead:
			return ImageState::ShaderRead();
		case BHive::EImageAccess::DepthWrite:
			return ImageState::DepthStencilAttachment();
		case BHive::EImageAccess::ComputeSampled:
			return ImageState::ShaderRead();
		case BHive::EImageAccess::ComputeStorageRead:
			return ImageState::ComputeRead();
		case BHive::EImageAccess::ComputeStorageWrite:
			return ImageState::ComputeWrite();
		case BHive::EImageAccess::TransferSrc:
			return ImageState::TransferRead();
		case BHive::EImageAccess::TransferDst:
			return ImageState::TransferWrite();
		default:
			return Undefined();
		}
	}
}
