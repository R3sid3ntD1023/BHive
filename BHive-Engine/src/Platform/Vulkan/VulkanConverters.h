#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/TextureSpecification.h"


namespace BHive
{
	struct FVulkanTextureCreateInfo
	{
		vk::Format Format{};
		vk::Filter MinFilter{};
		vk::Filter MagFilter{};
		vk::ImageUsageFlags Usage{};
		vk::ImageAspectFlags Aspect{};
		vk::SamplerAddressMode WrapMode{};
		vk::Bool32 CompareEnabled{};
		vk::CompareOp CompareOp{};
		uint32_t BytesPerPixel{};
		uint32_t MipLevels{};
		uint32_t ArrayLayers{};
		std::string DebugName{};
	};

	vk::BufferUsageFlags ToVkBufferType(EBufferType type);

	vk::DescriptorType ToVkType(EResourceType type);

	vk::ImageAspectFlags ToVkAspect(ETextureAspect aspect);

	vk::ImageUsageFlags ToVKImageUsage(ETextureUsage usage);

	vk::PrimitiveTopology ToVkTopology(ETopologyMode Mode);

	vk::PolygonMode ToVkPolygon(EPolygonMode mode);

	vk::CullModeFlagBits ToVkCull(ECullMode mode);

	vk::FrontFace ToVkFrontFace(EFrontFace face);

	vk::CompareOp ToVkCompare(ECompareOp op);

	vk::BlendFactor ToVkBlendFactor(EBlendFactor factor);

	vk::BlendOp ToVkBlendOp(EBlendOp op);

	vk::Format ToVkFormat(EFormat format);

	vk::Filter ToVkFilter(EMinFilter mode);

	vk::Filter ToVkFilter(EMagFilter mode);

	vk::SamplerAddressMode ToVkWrap(EWrapMode mode);

	vk::ShaderStageFlags ToVkShaderStageBit(EShaderStage stage);

	vk::ShaderStageFlagBits ToSingleVkStage(EShaderStage stage);

	FVulkanTextureCreateInfo Convert(const FTextureCreateInfo &info);
}