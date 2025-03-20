#pragma once

#include "core/reflection/Reflection.h"
#include "Color.h"
#include <optional>
#include <stdint.h>

namespace BHive
{
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

	enum class ETextureFormat
	{
		R,
		RG,
		RGB,
		RGBA
	};

	enum class EFormat
	{
		Invalid,
		R8,
		R8F,
		R32F,
		RG8,
		RG32F,
		RG16F,
		RGB8,
		RGBA8,
		RGB16F,
		RGBA32F,
		RGBA16F,
		RGB32F,
		RED_INTEGER,
		RGB_UINTEGER,
		RGB_INTEGER,
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

	enum class ETextureCompareMode
	{
		NONE,
		COMPARE_REF_TO_TEXTURE,

	};

	enum class ETextureCompareFunc
	{
		LEQUAL,
		GEQUAL,
		LESS,
		GREATER,
		EQUAL,
		NOTEQUAL,
		ALWAYS,
		NEVER
	};

	enum class EImageAccess : uint32_t
	{
		READ,
		WRITE,
		READ_WRITE
	};

	struct FTextureSpecification
	{
		uint32_t Channels = 0;
		EFormat InternalFormat = EFormat::Invalid;
		ETextureFormat Format = ETextureFormat::RGBA;
		EWrapMode WrapMode = EWrapMode::REPEAT;
		EMinFilter MinFilter = EMinFilter::LINEAR;
		EMagFilter MagFilter = EMagFilter::LINEAR;
		FColor BorderColor = 0xFFFFFFFF;
		uint32_t Levels = 1;
		std::optional<EImageAccess> ImageAccess;
		std::optional<ETextureCompareMode> CompareMode; // Depth Compare Mode
		std::optional<ETextureCompareFunc> CompareFunc; // Depth Compare Funcs

		REFLECTABLE()
	};

	REFLECT_EXTERN(FTextureSpecification)

	template <typename A>
	void Serialize(A &ar, FTextureSpecification &spec)
	{
		ar(MAKE_NVP(spec.Format), MAKE_NVP(spec.InternalFormat), MAKE_NVP(spec.Channels), MAKE_NVP(spec.WrapMode),
		   MAKE_NVP(spec.MinFilter), MAKE_NVP(spec.MagFilter), MAKE_NVP(spec.BorderColor), MAKE_NVP(spec.Levels),
		   MAKE_NVP(spec.CompareMode), MAKE_NVP(spec.CompareFunc));
	}

} // namespace BHive
