#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "TextureSpecification.h"

namespace BHive
{

	class BHIVE Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void SetWrapMode(EWrapMode mode) {}
		virtual void SetMinFilter(EMinFilter mode) {};
		virtual void SetMagFilter(EMagFilter mode) {};

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind(uint32_t slot = 0) const = 0;

		virtual void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0) = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual uint64_t GetResourceHandle() const = 0;
		virtual uint64_t GetImageHandle() const = 0;

		virtual void GenerateMipMaps() const = 0;
		virtual const FTextureSpecification &GetSpecification() const = 0;

		operator uint32_t() const { return GetRendererID(); }

		REFLECTABLEV()
	};

	class BHIVE Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		static Ref<Texture2D> Create();
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, const FTextureSpecification &specification, uint32_t samples = 1);
		static Ref<Texture2D> Create(const void *data, uint32_t width, uint32_t height, const FTextureSpecification &specification);

		REFLECTABLEV(Texture)
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(uint32_t size, const FTextureSpecification &spec);
	};

	class Texture3D : public Texture
	{
	public:
		static Ref<Texture> Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureSpecification &specification);
	};

	REFLECT_EXTERN(Texture)
	REFLECT_EXTERN(Texture2D)
} // namespace BHive
