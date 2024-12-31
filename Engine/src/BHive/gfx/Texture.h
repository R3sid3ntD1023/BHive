#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "gfx/Color.h"

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
		LUMINANCE,
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

	struct BHIVE FTextureSpecification
	{
		uint32_t Channels;
		EFormat Format;
		EWrapMode WrapMode;
		EFilterMode MinFilter = EFilterMode::LINEAR, MagFilter = EFilterMode::LINEAR;
		Color BorderColor = 0xFFFFFFFF;
		bool Mips = true;
		unsigned Levels = 1;
		ETextureType Type = ETextureType::TEXTURE_2D;

		// depth
		std::optional<ETextureCompareMode> CompareMode;
		std::optional<ETextureCompareFunc> CompareFunc;
	};

	class BHIVE Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const = 0;

		virtual void UnBind(uint32_t slot = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0) = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual void GenerateMipMaps() const = 0;

		operator uint32_t() const { return GetRendererID(); }

		virtual uint64_t GetResourceHandle() const { return 0; };

		ASSET_CLASS(Texture)
		REFLECTABLEV()
	};

	class BHIVE Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples = 1);
		static Ref<Texture2D> Create(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification);

		REFLECTABLEV(Texture)
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(uint32_t size, const FTextureSpecification &spec);

		virtual void AttachToFramebuffer(const Ref<class Framebuffer> &framebuffer, uint32_t attachment, uint32_t level) = 0;
	};

	class Texture3D : public Texture
	{
	public:
		static Ref<Texture> Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification);
	};

	REFLECT_EXTERN(Texture)
	REFLECT_EXTERN(Texture2D)
}
