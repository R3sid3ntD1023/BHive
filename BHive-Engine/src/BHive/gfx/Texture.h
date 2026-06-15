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

	struct FTextureUploadInfo
	{
		const void *Data = nullptr;
		glm::ivec3 Offset = {0, 0, 0};
		glm::uvec3 Extent = {0, 0, 1}; // if 0, texture decides full size 
		uint32_t BaseMipLevel = 0; 
		uint32_t Levels = 1;
		uint32_t BaseArrayLayer = 0; 
		uint32_t Layers = 1;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual void GenerateMips() = 0;

		virtual glm::uvec2 GetSize() const = 0;

		float GetAspectRatio() const { return (float)GetSize().x / (float)GetSize().y; }

		virtual void SetData(const FTextureUploadInfo& info) = 0;

		virtual const FTextureCreateInfo &GetInfo() const = 0;

		virtual NativeHandle GetNativeHandle() const = 0;

		virtual NativeHandle GetRenderView(uint32_t layer = 0, uint32_t mip = 0) const = 0;

		REFLECTABLEV(Asset)
	};

	class BHIVE_API Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual Ref<Texture2D> CreateSubTexture(const FSubTexture &subTexture) = 0;

		virtual const FTextureCreateInfo &GetInfo() const = 0;

		virtual void SetInfo(const FTextureCreateInfo &specs) = 0;

		virtual const Buffer &GetBuffer() const = 0;

		static Ref<Texture2D> Create();

		static Ref<Texture2D> Create(const glm::uvec2 &size, const FTextureCreateInfo &info = {}, const Buffer &data = {});

		REFLECTABLEV(Texture)

	private:
		int32_t mLayerIndex = -1; //used by texture2d array

		friend class Texture2DArray;
	};

	class BHIVE_API Texture2DArray : public Texture
	{
	public:
		virtual ~Texture2DArray() = default;

		void SetStartLayer(uint32_t layer) { mStartLayer = layer; }

		int32_t AddTexture(const Ref<Texture2D> &tex);

		Ref<Texture2D> GetTexture(uint32_t index) const;

		void Clear();

		static Ref<Texture2DArray> Create(const glm::uvec2& size, const FTextureCreateInfo &createInfo);

	private:
		uint32_t mCurrentLayer = 0;
		uint32_t mStartLayer = 0;
		std::vector<Ref<Texture2D>> mStoredTextures;
	};

	class BHIVE_API Texture3D : public Texture
	{
	public:
		virtual ~Texture3D() = default;

		static Ref<Texture3D> Create(const glm::uvec3 &size, const FTextureCreateInfo &createInfo, const Buffer &data = {});
	};

	class BHIVE_API TextureCube : public Texture
	{
	public:
		virtual ~TextureCube() = default;

		static Ref<TextureCube> Create(uint32_t size, const FTextureCreateInfo &createInfo);
	};

	class BHIVE_API TextureCubeArray : public Texture
	{
	public:
		virtual ~TextureCubeArray() = default;

		static Ref<TextureCubeArray> Create(uint32_t size, const FTextureCreateInfo &createInfo);
	};


	REFLECT_EXTERN(Texture);
	REFLECT_EXTERN(Texture2D);

} // namespace BHive
