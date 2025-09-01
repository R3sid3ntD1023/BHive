#pragma once

#include "Color.h"
#include "core/reflection/Reflection.h"
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

	struct FTextureAPIInfo
	{
		uint32_t Levels = 1;
		uint32_t InternalFormat = 0;
		uint32_t Format = 0;
		uint32_t Type = 0;

		uint32_t WrapMode = 0;
		uint32_t FilterModes[2] = {};
		uint32_t GenerateMipMaps = 0;

		uint32_t CompareMode = 0;
		uint32_t CompareFunc = 0;

		float BorderColor[4] = {};

		uint32_t IsDepth = 0;
	};

	struct FTextureCreateInfo
	{
		uint32_t Channels = 0;
		EFormat InternalFormat = EFormat::Invalid;
		EWrapMode WrapMode = EWrapMode::REPEAT;
		EMinFilter MinFilter = EMinFilter::LINEAR;
		EMagFilter MagFilter = EMagFilter::LINEAR;
		FColor BorderColor = 0xFFFFFFFF;
		uint32_t Levels = 1;
		uint32_t GenerateMipMaps = 0;
		std::optional<ETextureCompareMode> CompareMode; // Depth Compare Mode
		std::optional<ETextureCompareFunc> CompareFunc; // Depth Compare Funcs

		operator FTextureAPIInfo() const;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(InternalFormat));
			ar(MAKE_NVP(Channels));
			ar(MAKE_NVP(WrapMode));
			ar(MAKE_NVP(MinFilter));
			ar(MAKE_NVP(MagFilter));
			ar(MAKE_NVP(BorderColor));
			ar(MAKE_NVP(Levels));
			ar(MAKE_NVP(CompareMode));
			ar(MAKE_NVP(CompareFunc));
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FTextureCreateInfo)

} // namespace BHive
