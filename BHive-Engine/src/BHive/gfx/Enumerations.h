#pragma once

#include "core/Core.h"
#include "core/EnumFlags.h"

namespace BHive
{
	enum class ETopologyMode
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	enum class EPolygonMode
	{
		Fill,
		Line,
		Point
	};

	enum class ECullMode
	{
		None,
		Front,
		Back,
		FrontAndBack
	};

	enum class EFrontFace
	{
		CW,
		CWW
	};

	enum class ECompareOp
	{
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always
	};

	enum class EBlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha
	};

	enum class EBlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class EFormat
	{
		None,
		R8,
		RG8,
		RGB8,
		RGBA8,
		R16F,
		RG16F,
		RGB16F,
		RGBA16F,
		R32F,
		RG32F,
		RGB32F,
		RGBA32F,
		RED_INTEGER,
		RGB_UINTEGER,
		RGB_INTEGER,
		RGBA_INTEGER,
		R11_G11_B10,
		DEPTH24_STENCIL8,
		DEPTH_COMPONENT,
		DEPTH_COMPONENT_32F,
		DEPTH_COMPONENT_24,
		Depth = DEPTH24_STENCIL8
	};

	enum class ETextureAspect
	{
		Color,
		DepthStencil
	};

	enum class ETextureRole : uint32_t
	{
		None = 0,
		Sampled = BIT(0),
		ComputeWrite = BIT(1),
		RenderTarget = BIT(2),
		DepthTarget = BIT(3),
		TransferSrc = BIT(4),
		TransferDst = BIT(5)
	};

	ENABLE_BITMASK_OPERATORS(ETextureRole)

	enum class ETextureType
	{
		TEXTURE_1D,
		TEXTURE_1D_ARRAY,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_2D_ARRAY,
		TEXTURE_CUBE_MAP,
		TEXTURE_CUBE_MAP_ARRAY,
		TEXTURE_RECTANGLE
	};

	enum class ECompareMode
	{
		NONE,
		COMPARE_REF_TO_TEXTURE,

	};

	enum class EImageUsage : uint8_t
	{
		None,

		// Graphics
		ColorRead,
		ColorWrite,

		DepthRead,
		DepthWrite,

		// Compute
		ComputeSampled,
		ComputeStorageRead,
		ComputeStorageWrite,

		// Transfer
		TransferSrc,
		TransferDst

	};

	enum class EBufferUsage : uint8_t
	{
		None,		   //= 0,
		TransferWrite, //= BIT(0),
		TransferRead,  //= BIT(1),
		VertexRead,	   //= BIT(2),
		IndexRead,	   //= BIT(3),
		UniformRead,   //= BIT(4),
		StorageRead,   //= BIT(5),
		StorageWrite,  //= BIT(6),
		IndirectRead,  //= BIT(7),
	};

	// ENABLE_BITMASK_OPERATORS(EBufferAccess)

	enum class EMagFilter
	{
		LINEAR,
		NEAREST
	};

	enum class EMinFilter
	{
		LINEAR,
		NEAREST,
		MIPMAP_LINEAR,
		MIPMAP_NEAREST,
		MIPMAP_LINEAR_NEAREST,
		MIPMAP_NEAREST_LINEAR
	};

	enum class EWrapMode
	{
		REPEAT,
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		CLAMP_TO_BORDER
	};

	enum class ClearMask : uint32_t
	{
		None = 0,
		DepthStencil = 1 << 0,
		Color = 1 << 1,
		All = Color | DepthStencil
	};

	ENABLE_BITMASK_OPERATORS(ClearMask)

	enum class EShaderStage : uint32_t
	{
		None = 0,
		Vertex = 1 << 0,
		Fragment = 1 << 1,
		Compute = 1 << 2,
		Geometry = 1 << 3,
	};

	ENABLE_BITMASK_OPERATORS(EShaderStage)

	enum class EShaderDataType
	{
		Float,
		Float2,
		Float3,
		Float4,
		UInt,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Mat3,
		Mat4
	};

	inline uint32_t GetBytesPerPixel(EFormat f)
	{
		switch (f)
		{
		case BHive::EFormat::None:
			break;
		case BHive::EFormat::R8:
		case BHive::EFormat::R16F:
		case BHive::EFormat::R32F:
		case BHive::EFormat::RED_INTEGER:
			return 1;
		case BHive::EFormat::RG8:
		case BHive::EFormat::RG16F:
		case BHive::EFormat::RG32F:
			return 2;
		case BHive::EFormat::RGB8:
		case BHive::EFormat::RGB16F:
		case BHive::EFormat::RGB32F:
		case BHive::EFormat::RGB_UINTEGER:
		case BHive::EFormat::RGB_INTEGER:
		case BHive::EFormat::R11_G11_B10:
			return 3;
		case BHive::EFormat::RGBA8:
		case BHive::EFormat::RGBA16F:
		case BHive::EFormat::RGBA32F:
		case BHive::EFormat::RGBA_INTEGER:
			return 4;
		case BHive::EFormat::DEPTH24_STENCIL8:
		case BHive::EFormat::DEPTH_COMPONENT:
		case BHive::EFormat::DEPTH_COMPONENT_32F:
		case BHive::EFormat::DEPTH_COMPONENT_24:
		default:
			break;
		}

		return 0;
	}

	enum class EResourceType
	{
		Invalid,
		CombinedImageSampler,
		StorageImage,
		SeperatedImage,
		SeperatedSampler,
		InputAttachment,
		UniformBuffer,
		StorageBuffer
	};

	enum class EBufferType : uint8_t
	{
		Undefined = 0,
		UniformBuffer = BIT(0),
		StorageBuffer = BIT(1),
		IndirectBuffer = BIT(2)
	};

	ENABLE_BITMASK_OPERATORS(EBufferType)

	enum class EResourceCategory : uint32_t
	{
		None = 0,
		Buffer = BIT(0),
		Texture = BIT(1),
		Sampler = BIT(2)
	};

	ENABLE_BITMASK_OPERATORS(EResourceCategory)

	inline EResourceCategory GetCategory(EResourceType t)
	{
		switch (t)
		{
		case BHive::EResourceType::UniformBuffer:
		case BHive::EResourceType::StorageBuffer:
			return EResourceCategory::Buffer;

		case BHive::EResourceType::CombinedImageSampler:
		case BHive::EResourceType::StorageImage:
		case BHive::EResourceType::SeperatedImage:
		case BHive::EResourceType::InputAttachment:
			return EResourceCategory::Texture;

		case BHive::EResourceType::SeperatedSampler:
			return EResourceCategory::Sampler;

		default:
			return EResourceCategory::None;
		}
	};

	inline EBufferType BufferTypeFromResource(EResourceType t)
	{
		switch (t)
		{
		case BHive::EResourceType::UniformBuffer:
			return EBufferType::UniformBuffer;
		case BHive::EResourceType::StorageBuffer:
			return EBufferType::StorageBuffer;
		default:
			return EBufferType::Undefined;
		}
	}

	inline const char *ToString(EResourceType type)
	{
		switch (type)
		{
		case BHive::EResourceType::CombinedImageSampler:
			return "CombinedImageSampler";
		case BHive::EResourceType::StorageImage:
			return "StorageImage";
		case BHive::EResourceType::SeperatedImage:
			return "SeperatedImage";
		case BHive::EResourceType::SeperatedSampler:
			return "SeperatedSampler";
		case BHive::EResourceType::UniformBuffer:
			return "UniformBuffer";
		case BHive::EResourceType::StorageBuffer:
			return "StorageBuffer";
		default:
			return "";
		}
	}

	inline std::string to_string(EImageUsage access)
	{
		switch (access)
		{
		case BHive::EImageUsage::ColorRead:
			return "ColorRead";
		case BHive::EImageUsage::ColorWrite:
			return "ColorWrite";
		case BHive::EImageUsage::DepthRead:
			return "DepthRead";
		case BHive::EImageUsage::DepthWrite:
			return "DepthWrite";
		case BHive::EImageUsage::ComputeSampled:
			return "ComputeSampled";
		case BHive::EImageUsage::ComputeStorageRead:
			return "ComputeStorageRead";
		case BHive::EImageUsage::ComputeStorageWrite:
			return "ComputeStorageWrite";
		case BHive::EImageUsage::TransferSrc:
			return "TransferSrc";
		case BHive::EImageUsage::TransferDst:
			return "TransferDst";
		default:
			return "Undefined";
		}
	}

	inline bool IsDepthFormat(EFormat format)
	{
		return format == EFormat::DEPTH24_STENCIL8 || format == EFormat::DEPTH_COMPONENT || format == EFormat::DEPTH_COMPONENT_24 || format == EFormat::DEPTH_COMPONENT_32F;
	}

	inline bool IsTexture(EResourceType type)
	{
		return (GetCategory(type) & EResourceCategory::Texture) != EResourceCategory::None;
	}

	inline bool IsBuffer(EResourceType type)
	{
		return (GetCategory(type) & EResourceCategory::Buffer) != EResourceCategory::None;
	}

} // namespace BHive