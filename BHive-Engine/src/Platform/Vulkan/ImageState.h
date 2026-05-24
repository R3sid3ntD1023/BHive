#pragma once

#include "VulkanCore.h"

namespace BHive
{
	struct ImageState
	{
		vk::ImageLayout Layout = vk::ImageLayout::eUndefined;

		vk::AccessFlags2 Access = {};

		vk::PipelineStageFlags2 Stage = {};

		bool IsUndefined = true;

		ImageState() = default;

		ImageState(vk::ImageLayout layout, vk::AccessFlags2 access, vk::PipelineStageFlags2 stage, bool unDefined = false)
			: Layout(layout),
			  Access(access),
			  Stage(stage),
			  IsUndefined(Undefined)
		{
		}

		//only for first transition after creation
		static ImageState Undefined() { return {vk::ImageLayout::eUndefined, {}, vk::PipelineStageFlagBits2::eTopOfPipe}; }

		//swapchain presentable images only
		static ImageState Present() { return {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe}; }


		static ImageState ColorAttachment()
		{
			return {
				vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput};
		}

		static ImageState DepthStencilAttachment()
		{
			return {
				vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
				vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests};
		}

		static ImageState ShaderRead()
		{
			return {
				vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eShaderRead,
				vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader};
		}

		static ImageState ComputeWrite() { return {vk::ImageLayout::eGeneral, vk::AccessFlagBits2::eShaderRead | vk::AccessFlagBits2::eShaderWrite, vk::PipelineStageFlagBits2::eComputeShader}; }

		static ImageState TansferRead() { return {vk::ImageLayout::eTransferSrcOptimal, vk::AccessFlagBits2::eTransferRead, vk::PipelineStageFlagBits2::eTransfer}; }

		static ImageState TansferWrite() { return {vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eTransfer}; }
	};
}