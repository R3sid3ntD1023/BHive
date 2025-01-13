#pragma once

#include <optional>
#include <stdint.h>
#include "Color.h"

namespace BHive
{
	enum class EFilterMode
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

	enum class EFormat
	{
		Invalid,
		R8,
		R8F,
		RG8,
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
		TEXTURE_ARRAY_2D,
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

	struct FTextureSpecification
	{
		uint32_t mChannels;
		EFormat mFormat;
		EWrapMode mWrapMode = EWrapMode::REPEAT;
		EFilterMode mMinFilter = EFilterMode::LINEAR, mMagFilter = EFilterMode::LINEAR;
		Color mBorderColor = 0xFFFFFFFF;
		bool mMips = false;
		unsigned mLevels = 1;
		ETextureType mType = ETextureType::TEXTURE_2D;

		// depth
		std::optional<ETextureCompareMode> mCompareMode;
		std::optional<ETextureCompareFunc> mCompareFunc;
	};
} // namespace BHive
