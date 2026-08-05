#pragma once

#include "VulkanCore.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	inline vk::DescriptorType ToVkType(EResourceType type)
	{
		switch (type)
		{
		case EResourceType::CombinedImageSampler:
			return vk::DescriptorType::eCombinedImageSampler;
		case EResourceType::StorageImage:
			return vk::DescriptorType::eStorageImage;
		case EResourceType::UniformBuffer:
			return vk::DescriptorType::eUniformBuffer;
		case EResourceType::StorageBuffer:
			return vk::DescriptorType::eStorageBuffer;
		case EResourceType::InputAttachment:
			return vk::DescriptorType::eInputAttachment;
		case EResourceType::SeperatedSampler:
			return vk::DescriptorType::eSampler;
		case EResourceType::SeperatedImage:
			return vk::DescriptorType::eSampledImage;
		default:
			ASSERT(false, "Unknown ResourceType {}", ToString(type));
			return vk::DescriptorType::eCombinedImageSampler;
		}
	}

	inline vk::ImageAspectFlags ToVkAspect(ETextureAspect aspect)
	{
		switch (aspect)
		{
		case BHive::ETextureAspect::Color:
			return vk::ImageAspectFlagBits::eColor;
		case BHive::ETextureAspect::DepthStencil:
			return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		default:
			break;
		}

		ASSERT(false);
		return {};
	}

	inline vk::ImageUsageFlags InferImageUsage(ETextureRole roles)
	{
		vk::ImageUsageFlags flags{};

		if ((roles & ETextureRole::Sampled) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eSampled;

		if ((roles & ETextureRole::RenderTarget) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eColorAttachment;

		if ((roles & ETextureRole::DepthTarget) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;

		if ((roles & ETextureRole::TransferSrc) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eTransferSrc;

		if ((roles & ETextureRole::TransferDst) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eTransferDst;

		if ((roles & ETextureRole::ComputeWrite) != ETextureRole::None)
			flags |= vk::ImageUsageFlagBits::eStorage; // compute writes require storage

		return flags;
	}

	inline vk::PrimitiveTopology ToVkTopology(ETopologyMode m)
	{
		switch (m)
		{
		case ETopologyMode::Points:
			return vk::PrimitiveTopology::ePointList;
		case ETopologyMode::Lines:
			return vk::PrimitiveTopology::eLineList;
		case ETopologyMode::LineStrip:
			return vk::PrimitiveTopology::eLineStrip;
		case ETopologyMode::Triangles:
			return vk::PrimitiveTopology::eTriangleList;
		case ETopologyMode::TriangleStrip:
			return vk::PrimitiveTopology::eTriangleStrip;
		case ETopologyMode::TriangleFan:
			return vk::PrimitiveTopology::eTriangleFan;
		}

		return vk::PrimitiveTopology::eTriangleList;
	}

	inline vk::PolygonMode ToVkPolygon(EPolygonMode m)
	{
		switch (m)
		{
		case EPolygonMode::Fill:
			return vk::PolygonMode::eFill;
		case EPolygonMode::Line:
			return vk::PolygonMode::eLine;
		case EPolygonMode::Point:
			return vk::PolygonMode::ePoint;
		}

		return vk::PolygonMode::eFill;
	}

	inline vk::CullModeFlagBits ToVkCull(ECullMode m)
	{
		switch (m)
		{
		case ECullMode::None:
			return vk::CullModeFlagBits::eNone;
		case ECullMode::Front:
			return vk::CullModeFlagBits::eFront;
		case ECullMode::Back:
			return vk::CullModeFlagBits::eBack;
		case ECullMode::FrontAndBack:
			return vk::CullModeFlagBits::eFrontAndBack;
		}
		return vk::CullModeFlagBits::eNone;
	}

	inline vk::FrontFace ToVkFrontFace(EFrontFace f)
	{
		return (f == EFrontFace::CW) ? vk::FrontFace::eClockwise : vk::FrontFace::eCounterClockwise;
	}

	inline vk::CompareOp ToVkCompare(ECompareOp op)
	{
		switch (op)
		{
		case ECompareOp::Never:
			return vk::CompareOp::eNever;
		case ECompareOp::Less:
			return vk::CompareOp::eLess;
		case ECompareOp::Equal:
			return vk::CompareOp::eEqual;
		case ECompareOp::LessOrEqual:
			return vk::CompareOp::eLessOrEqual;
		case ECompareOp::Greater:
			return vk::CompareOp::eGreater;
		case ECompareOp::NotEqual:
			return vk::CompareOp::eNotEqual;
		case ECompareOp::GreaterOrEqual:
			return vk::CompareOp::eGreaterOrEqual;
		case ECompareOp::Always:
			return vk::CompareOp::eAlways;
		}
		return vk::CompareOp::eAlways;
	}

	inline vk::BlendFactor ToVkBlendFactor(EBlendFactor f)
	{
		switch (f)
		{
		case EBlendFactor::Zero:
			return vk::BlendFactor::eZero;
		case EBlendFactor::One:
			return vk::BlendFactor::eOne;
		case EBlendFactor::SrcColor:
			return vk::BlendFactor::eSrcColor;
		case EBlendFactor::OneMinusSrcColor:
			return vk::BlendFactor::eOneMinusSrcColor;
		case EBlendFactor::DstColor:
			return vk::BlendFactor::eDstColor;
		case EBlendFactor::OneMinusDstColor:
			return vk::BlendFactor::eOneMinusDstColor;
		case EBlendFactor::SrcAlpha:
			return vk::BlendFactor::eSrcAlpha;
		case EBlendFactor::OneMinusSrcAlpha:
			return vk::BlendFactor::eOneMinusSrcAlpha;
		case EBlendFactor::DstAlpha:
			return vk::BlendFactor::eDstAlpha;
		case EBlendFactor::OneMinusDstAlpha:
			return vk::BlendFactor::eOneMinusDstAlpha;
		}
		return vk::BlendFactor::eOne;
	}

	inline vk::BlendOp ToVkBlendOp(EBlendOp op)
	{
		switch (op)
		{
		case EBlendOp::Add:
			return vk::BlendOp::eAdd;
		case EBlendOp::Subtract:
			return vk::BlendOp::eSubtract;
		case EBlendOp::ReverseSubtract:
			return vk::BlendOp::eReverseSubtract;
		case EBlendOp::Min:
			return vk::BlendOp::eMin;
		case EBlendOp::Max:
			return vk::BlendOp::eMax;
		}
		return vk::BlendOp::eAdd;
	}

	inline vk::Format ToVkFormat(EFormat f)
	{
		switch (f)
		{
		case EFormat::None:
			return vk::Format::eUndefined;
		case EFormat::R8:
			return vk::Format::eR8Unorm;
		case EFormat::RG8:
			return vk::Format::eR8G8Unorm;
		case EFormat::RGB8:
			return vk::Format::eR8G8B8Unorm;
		case EFormat::RGBA8:
			return vk::Format::eR8G8B8A8Unorm;
		case EFormat::R16F:
			return vk::Format::eR16Sfloat;
		case EFormat::RG16F:
			return vk::Format::eR16G16Sfloat;
		case EFormat::RGB16F:
			return vk::Format::eR16G16B16Sfloat;
		case EFormat::RGBA16F:
			return vk::Format::eR16G16B16A16Sfloat;
		case EFormat::R32F:
			return vk::Format::eR32Sfloat;
		case EFormat::RG32F:
			return vk::Format::eR32G32Sfloat;
		case EFormat::RGB32F:
			return vk::Format::eR32G32B32Sfloat;
		case EFormat::RGBA32F:
			return vk::Format::eR32G32B32A32Sfloat;
		case EFormat::RED_INTEGER:
			return vk::Format::eR32Sint;
		case EFormat::RGB_UINTEGER:
			return vk::Format::eR32G32B32Uint;
		case EFormat::RGB_INTEGER:
			return vk::Format::eR32G32B32Sint;
		case EFormat::RGBA_INTEGER:
			return vk::Format::eR32G32B32A32Sint;
		case EFormat::R11_G11_B10:
			return vk::Format::eB10G11R11UfloatPack32;
		case EFormat::DEPTH24_STENCIL8:
			return vk::Format::eD24UnormS8Uint;
		case EFormat::DEPTH_COMPONENT:
		case EFormat::DEPTH_COMPONENT_24:
			return vk::Format::eD24UnormS8Uint;
		case EFormat::DEPTH_COMPONENT_32F:
			return vk::Format::eD32Sfloat;
		default:
			return vk::Format::eUndefined;
		}
	}

	inline vk::Filter ToVkFilter(EMinFilter f)
	{
		switch (f)
		{
		case EMinFilter::LINEAR:
			return vk::Filter::eLinear;
		case EMinFilter::NEAREST:
			return vk::Filter::eNearest;
		default:
			return vk::Filter::eLinear;
		}
	}

	inline vk::Filter ToVkFilter(EMagFilter f)
	{
		switch (f)
		{
		case EMagFilter::LINEAR:
			return vk::Filter::eLinear;
		case EMagFilter::NEAREST:
			return vk::Filter::eNearest;
		}
		return vk::Filter::eLinear;
	}

	inline vk::SamplerAddressMode ToVkWrap(EWrapMode w)
	{
		switch (w)
		{
		case EWrapMode::REPEAT:
			return vk::SamplerAddressMode::eRepeat;
		case EWrapMode::CLAMP_TO_EDGE:
			return vk::SamplerAddressMode::eClampToEdge;
		case EWrapMode::MIRRORED_REPEAT:
			return vk::SamplerAddressMode::eMirroredRepeat;
		case EWrapMode::CLAMP_TO_BORDER:
			return vk::SamplerAddressMode::eClampToBorder;
		}
		return vk::SamplerAddressMode::eRepeat;
	}

	inline vk::ShaderStageFlags ToVkShaderStageBit(EShaderStage s)
	{
		vk::ShaderStageFlags flags{};

		if ((s & EShaderStage::Vertex) != EShaderStage::None)
			flags |= vk::ShaderStageFlagBits::eVertex;

		if ((s & EShaderStage::Fragment) != EShaderStage::None)
			flags |= vk::ShaderStageFlagBits::eFragment;

		if ((s & EShaderStage::Compute) != EShaderStage::None)
			flags |= vk::ShaderStageFlagBits::eCompute;

		if ((s & EShaderStage::Geometry) != EShaderStage::None)
			flags |= vk::ShaderStageFlagBits::eGeometry;

		return flags;
	}

	inline vk::ShaderStageFlagBits ToSingleVkStage(EShaderStage s)
	{
		switch (s)
		{
		case EShaderStage::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case EShaderStage::Fragment:
			return vk::ShaderStageFlagBits::eFragment;
		case EShaderStage::Compute:
			return vk::ShaderStageFlagBits::eCompute;
		case EShaderStage::Geometry:
			return vk::ShaderStageFlagBits::eGeometry;
		default:
			throw std::runtime_error("Invalid or multi-stage passed to ToSingleVkStage");
		}
	}

	inline vk::PipelineStageFlags2 ToStage(EBufferAccess a)
	{
		switch (a)
		{
		case BHive::EBufferAccess::TransferWrite:
		case BHive::EBufferAccess::TransferRead:
			return vk::PipelineStageFlagBits2::eTransfer;
		case BHive::EBufferAccess::VertexRead:
			return vk::PipelineStageFlagBits2::eVertexInput;
		case BHive::EBufferAccess::IndexRead:
			return vk::PipelineStageFlagBits2::eIndexInput;
		case BHive::EBufferAccess::UniformRead:
			return vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader;
		case BHive::EBufferAccess::StorageRead:
		case BHive::EBufferAccess::StorageWrite:
			return vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader;
		case BHive::EBufferAccess::IndirectRead:
			return vk::PipelineStageFlagBits2::eDrawIndirect;
		default:
			return vk::PipelineStageFlagBits2::eAllCommands;
		}
	}

	inline vk::AccessFlags2 ToAccess(EBufferAccess a)
	{
		switch (a)
		{
		case BHive::EBufferAccess::TransferWrite:
			return vk::AccessFlagBits2::eTransferWrite;
		case BHive::EBufferAccess::TransferRead:
			return vk::AccessFlagBits2::eTransferRead;
		case BHive::EBufferAccess::VertexRead:
			return vk::AccessFlagBits2::eVertexAttributeRead;
		case BHive::EBufferAccess::IndexRead:
			return vk::AccessFlagBits2::eIndexRead;
		case BHive::EBufferAccess::UniformRead:
			return vk::AccessFlagBits2::eUniformRead;
		case BHive::EBufferAccess::StorageRead:
			return vk::AccessFlagBits2::eShaderRead;
		case BHive::EBufferAccess::StorageWrite:
			return vk::AccessFlagBits2::eShaderWrite;
		case BHive::EBufferAccess::IndirectRead:
			return vk::AccessFlagBits2::eIndirectCommandRead;
		default:
			return vk::AccessFlagBits2::eNone;
		}
	}
} // namespace BHive