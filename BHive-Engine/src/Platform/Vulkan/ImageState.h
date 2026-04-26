#pragma once

#include "VulkanCore.h"

namespace BHive
{
	struct ImageState
	{
		vk::ImageLayout Layout = vk::ImageLayout::eUndefined;

		vk::AccessFlags2 Access = {};

		vk::PipelineStageFlags2 Stage = {};

		static ImageState ShaderRead()
		{
			return {vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderSampledRead, vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader};
		}

		static ImageState ComputeWrite() { return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eShaderWrite, vk::PipelineStageFlagBits2::eComputeShader}; }

		static ImageState TansferDst() { return {vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer}; }

		static ImageState ColorAttachment() { return {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput}; }

		static ImageState DepthStencilAttachmentment()
		{
			return {
				vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
				vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests};
		}

		static ImageState Present() { return {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe}; }
	};
}