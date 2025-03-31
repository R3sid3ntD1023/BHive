#pragma once

#include "gfx/Texture.h"

namespace BHive
{
	DECLARE_CLASS()
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

		DECLARE_FUNCTION()
		virtual uint32_t GetWidth() const { return mWidth; }

		DECLARE_FUNCTION()
		virtual uint32_t GetHeight() const { return mHeight; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offsetX = 0, uint32_t offsetY = 0);
		virtual uint32_t GetRendererID() const { return mTextureID; }

		virtual void GenerateMipMaps() const;

		DECLARE_FUNCTION()
		virtual const FTextureSpecification &GetSpecification() const { return mSpecification; }

		DECLARE_FUNCTION()
		void SetSpecification(const FTextureSpecification &specs);

		Ref<Texture2D> CreateSubTexture(const FSubTexture &texture);

		void GetSubImage(const FSubTexture &texture, size_t size, uint8_t *data) const;

		/*Begin Asset*/
		void Save(cereal::BinaryOutputArchive &ar) const override;
		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Texture)

		/*End Asset*/

	private:
		void Initialize();
		void Release();

	private:
		DECLARE_PROPERTY(ReadOnly , Getter = GetWidth)
		uint32_t mWidth = 0;

		DECLARE_PROPERTY(ReadOnly, Getter = GetHeight)
		uint32_t mHeight = 0;

		DECLARE_PROPERTY(Setter = SetSpecification, Getter = GetSpecification)
		FTextureSpecification mSpecification;
		uint32_t mTextureID = 0;
	};

} // namespace BHive