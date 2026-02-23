#pragma once

#include "asset/Asset.h"
#include "core/Buffer.h"
#include "core/Core.h"
#include "TextureSpecification.h"
#include "NativeHandle.h"

namespace BHive
{
	struct FSubTexture
	{
		glm::uvec3 Offset = {0, 0, 0};
		glm::uvec3 Size = {0, 0, 1};
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual void UnBind(uint32_t slot = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;

		virtual uint32_t GetHeight() const = 0;

		float GetAspectRatio() const { return (float)GetWidth() / (float)GetHeight(); }

		virtual void SetData(const void *data, const glm::uvec3 &offset = {0, 0, 0}) = 0;

		virtual const FTextureCreateInfo &GetInfo() const = 0;

		virtual NativeHandle GetNativeHandle() const = 0;

		REFLECTABLEV(Asset)
	};

	class BHIVE_API Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual Ref<Texture2D> CreateSubTexture(const FSubTexture &subTexture) = 0;

		virtual const FTextureCreateInfo &GetInfo() const override = 0;

		virtual void SetInfo(const FTextureCreateInfo &specs) = 0;

		virtual const Buffer &GetBuffer() const = 0;

		static Ref<Texture2D> Create();

		static Ref<Texture2D> Create(uint32_t w, uint32_t h, const FTextureCreateInfo &info = {}, const void *buffer = nullptr, size_t size = 0);

		REFLECTABLEV(Texture)

	private:
		uint32_t mLayerIndex; //used by texture2d array

		friend class Tetxure2DArray;
	};

	class BHIVE_API Texture2DArray : public Texture
	{
	public:
		struct LayerInfo
		{
			glm::vec2 UvScale = {1.f, 1.f};
			glm::uvec2 OrginalSize = {0, 0};
		};
	public:
		virtual ~Texture2DArray() = default;

		virtual void AddTexture(const Ref<Texture2D> &tex);

		const LayerInfo& GetLayerInfo(uint32_t layer) const;

		static Ref<Texture2DArray> Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification);


	private:
		std::vector<LayerInfo> mLayerInfo;
	};

	class BHIVE_API Texture3D : public Texture
	{
	public:
		virtual ~Texture3D() = default;

		static Ref<Texture3D> Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &create_info, const void *data = nullptr);
	};

	class BHIVE_API TextureCube : public Texture
	{
	public:
		virtual ~TextureCube() = default;

		static Ref<TextureCube> Create(uint32_t size, const FTextureCreateInfo &info);
	};

	class BHIVE_API TextureCubeArray : public Texture
	{
	public:
		virtual ~TextureCubeArray() = default;

		static Ref<TextureCubeArray> Create(uint32_t width, uint32_t height, uint32_t depth, const FTextureCreateInfo &specification);
	};

	class BHIVE_API Texture2DMultisample : public Texture
	{
	public:
		virtual ~Texture2DMultisample() = default;

		static Ref<Texture2DMultisample> Create(uint32_t width, uint32_t height, uint32_t samples, const FTextureCreateInfo &create_info);
	};

	REFLECT_EXTERN(Texture);
	REFLECT_EXTERN(Texture2D);

} // namespace BHive
