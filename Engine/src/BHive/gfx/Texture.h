#pragma once

#include "asset/Asset.h"
#include "core/Buffer.h"
#include "core/Core.h"
#include "TextureSpecification.h"

namespace BHive
{
	struct FSubTexture
	{
		uint32_t x = 0, y = 0, z = 0;
		uint32_t width = 1, height = 1, depth = 1;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind(uint32_t slot = 0) const = 0;

		virtual void BindAsImage(uint32_t unit, EImageAccess access, uint32_t level = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;

		virtual uint32_t GetHeight() const = 0;

		float GetAspectRatio() const { return (float)GetWidth() / (float)GetHeight(); }

		virtual void SetData(const void *data, uint32_t offsetX = 0, uint32_t offsetY = 0) = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual void GenerateMipMaps() const = 0;

		virtual const FTextureCreateInfo &GetInfo() const = 0;

		operator uint32_t() const { return GetRendererID(); }

		REFLECTABLEV(Asset)
	};

	REFLECT_EXTERN(Texture)

} // namespace BHive
