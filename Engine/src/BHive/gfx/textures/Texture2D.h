#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(
			uint32_t width, uint32_t height, const FTextureSpecification &specification = {}, const void *data = nullptr);

		virtual ~Texture2D();

		virtual void Bind(uint32_t slot = 0) const;
		virtual void UnBind(uint32_t slot = 0) const;

		virtual void BindAsImage(uint32_t unit, EImageAccess access, uint32_t level = 0) const;

		virtual uint32_t GetWidth() const { return mWidth; }
		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);
		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual void GenerateMipMaps() const;
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }
		void SetSpecification(const FTextureSpecification &specs);

		Ref<Texture2D> CreateSubTexture(const FSubTexture &texture);

		void GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const;

		/*Begin Asset*/
		void Save(cereal::BinaryOutputArchive &ar) const override;
		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Texture)

		/*End Asset*/

	private:
		void Initialize();
		void Release();

	private:
		uint32_t mWidth = 0, mHeight = 0;
		FTextureSpecification mSpecification;
		uint32_t mTextureID = 0;
	};

	REFLECT_EXTERN(Texture2D)

} // namespace BHive