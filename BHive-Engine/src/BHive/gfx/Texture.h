#pragma once

#include "ResourceID.h"
#include "TextureSpecification.h"
#include "asset/Asset.h"
#include "core/Buffer.h"
#include "core/Core.h"
#include "gfx/registries/Handles.h"

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
		virtual ~Texture() = default; //{ mResourceID.Release(); }

		virtual glm::uvec2 GetSize() const { return {0, 0}; };

		float GetAspectRatio() const { return (float)GetSize().x / (float)GetSize().y; }

		virtual void SetData(const FTextureUploadInfo &info) {};

		virtual const FTextureCreateInfo &GetInfo() const = 0;

		virtual void DebugPrintState() {};

		virtual uint32_t GetTextureID() const = 0;

		REFLECTABLEV(Asset)
	};

	class BHIVE_API Texture2D : public Texture
	{
	public:
		Texture2D() = default;

		Texture2D(const glm::uvec2 &size, const FTextureCreateInfo &createInfo, const ByteBuffer &data = {});

		~Texture2D();

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetInfo(const FTextureCreateInfo &specs) { mCreateInfo = specs; }

		const ByteBuffer &GetBuffer() const { return mBuffer; }

		uint32_t GetTextureID() const override { return mTextureID; }

		REFLECTABLEV(Texture)

	private:
		int32_t mLayerIndex = -1; // used by texture2d array

		ByteBuffer mBuffer;

		FTextureCreateInfo mCreateInfo;

		glm::uvec2 mSize;

		int32_t mTextureID = -1;

		friend class Texture2DArray;
	};

	class BHIVE_API Texture2DArray : public Texture
	{
	public:
		Texture2DArray(const glm::uvec2 &size, const FTextureCreateInfo &specification);

		~Texture2DArray();

		glm::uvec2 GetSize() const override { return mSize; }

		void SetData(const FTextureUploadInfo &info) override;

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

		void SetStartLayer(uint32_t layer) { mStartLayer = layer; }

		int32_t Append(TexturePtr tex);

		TexturePtr GetTexture(uint32_t index) const;

		uint32_t GetTextureID() const override { return mTextureID; }

		void Clear();

	private:
		uint32_t mCurrentLayer = 0;
		uint32_t mStartLayer = 0;
		glm::uvec2 mSize;
		FTextureCreateInfo mCreateInfo;
		int32_t mTextureID = -1;
		std::vector<TexturePtr> mStoredTextures;
	};

	class BHIVE_API Texture3D : public Texture
	{
	public:
		Texture3D(const glm::uvec3 &size, const FTextureCreateInfo &createInfo, const ByteBuffer &data);

		~Texture3D();

		uint32_t GetTextureID() const override { return mTextureID; }

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

	private:
		FTextureCreateInfo mCreateInfo;
		int32_t mTextureID = -1;
	};

	class BHIVE_API TextureCube : public Texture
	{
	public:
		TextureCube(uint32_t size, const FTextureCreateInfo &createInfo);

		~TextureCube();

		uint32_t GetTextureID() const override { return mTextureID; }

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

	private:
		FTextureCreateInfo mCreateInfo;
		int32_t mTextureID = -1;
	};

	class BHIVE_API TextureCubeArray : public Texture
	{
	public:
		TextureCubeArray(uint32_t size, const FTextureCreateInfo &createInfo);

		~TextureCubeArray();

		uint32_t GetTextureID() const override { return mTextureID; }

		const FTextureCreateInfo &GetInfo() const override { return mCreateInfo; }

	private:
		glm::uvec2 mSize;
		FTextureCreateInfo mCreateInfo;
		int32_t mTextureID = -1;
	};

} // namespace BHive
