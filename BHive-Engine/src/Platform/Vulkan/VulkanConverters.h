#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/TextureSpecification.h"


namespace BHive
{

	vk::ImageUsageFlags InferImageUsage(ETextureRole roles);

	vk::DescriptorType ToVkType(EResourceType type);

	vk::ImageAspectFlags ToVkAspect(ETextureAspect aspect);

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
}