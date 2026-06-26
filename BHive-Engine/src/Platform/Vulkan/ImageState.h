#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	struct ImageState
	{
		vk::ImageLayout Layout = vk::ImageLayout::eUndefined;

		vk::AccessFlags2 Access = {};

		vk::PipelineStageFlags2 Stage = {};

		bool IsUndefined = true;

		ImageState() = default;

		ImageState(vk::ImageLayout layout, vk::AccessFlags2 access, vk::PipelineStageFlags2 stage, bool unDefined = false);

		bool operator==(const ImageState &other) const { return Layout == other.Layout; }

		bool operator!=(const ImageState &other) const { return Layout != other.Layout; }

		static ImageState Undefined();

		static ImageState Present();

		static ImageState ColorAttachment();

		static ImageState DepthStencilAttachment();

		static ImageState ShaderRead();

		static ImageState ComputeRead();

		static ImageState ComputeWrite();

		static ImageState TransferRead();

		static ImageState TansferWrite();

		static ImageState TransferClear();

		static ImageState ToImageState(EImageAccess access);
	};
}