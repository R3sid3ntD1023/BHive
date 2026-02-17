#pragma once

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

	enum class EImageAccess : uint32_t
	{
		READ,
		WRITE,
		READ_WRITE
	};

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
		None			= 0,
		DepthStencil	= 1 << 0,
		Color			= 1 << 1,
		All				= Color | DepthStencil
	};

	ENUM_FLAGS(ClearMask)

	enum class EShaderStage : uint32_t
	{
		None		= 0,
		Vertex		= 1 << 0,
		Fragment	= 1 << 1,
		Compute		= 1 << 2,
		Geometry	= 1 << 3,
	};

	ENUM_FLAGS(EShaderStage)

	enum class EShaderDataType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Mat3,
		Mat4
	};


	inline bool IsDepthFormat(EFormat format)
	{
		return format == EFormat::DEPTH24_STENCIL8 || format == EFormat::DEPTH_COMPONENT || format == EFormat::DEPTH_COMPONENT_24 || format == EFormat::DEPTH_COMPONENT_32F;
	}
}