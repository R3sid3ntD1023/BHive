#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "TextureSpecification.h"
#include "asset/Asset.h"

namespace BHive
{

	class BHIVE Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind(uint32_t slot = 0) const = 0;

		virtual void BindAsImage(uint32_t unit, uint32_t access, uint32_t level = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0) = 0;
		virtual uint64_t GetResourceHandle() const { return 0; }
		virtual uint32_t GetRendererID() const = 0;
		virtual uint64_t GetImageHandle() const { return 0; }

		virtual void GenerateMipMaps() const = 0;
		virtual const FTextureSpecification &GetSpecification() const = 0;

		operator uint32_t() const { return GetRendererID(); }

		REFLECTABLEV(Asset)
	};

} // namespace BHive
