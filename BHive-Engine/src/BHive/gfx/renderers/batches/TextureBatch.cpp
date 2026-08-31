#include "TextureBatch.h"
#include "gfx/Texture.h"
#include "gfx/factories/TextureFactory.h"

namespace BHive
{
	void TextureBatchData::Initialize()
	{
		FTextureCreateInfo create_info{};
		create_info.Format = EFormat::RGBA8;
		create_info.Aspect = ETextureAspect::Color;
		create_info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
		create_info.ArrayLayers = sMaxTextureCount;
		create_info.DebugName = "TextureArray_TexBatch";

		mTextureArray = TextureFactory::Create2DArray({512, 512}, create_info);

		std::vector<uint32_t> white(512 * 512, 0xFFFFFFFF);
		Buffer data(white.data(), white.size() * sizeof(uint32_t));
		FTextureCreateInfo info{};
		info.Format = EFormat::RGBA8;
		info.Aspect = ETextureAspect::Color;
		info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
		info.DebugName = "WhiteTexture_TexBatch";
		auto defaultTex = TextureFactory::Create2D({512, 512}, info, data);

		mTextureArray.As<Texture2DArray>()->Append(defaultTex);
		mTextureArray.As<Texture2DArray>()->SetStartLayer(1);
	}

	void TextureBatchData::Reset()
	{
		mTextureCount = 1;
	}

	bool TextureBatchData::IsFull()
	{
		return mTextureCount >= sMaxTextureCount;
	}

	int32_t TextureBatchData::GetTextureIndex(TexturePtr texture)
	{
		if (!texture)
			return 0;

		for (uint32_t i = 1; i < mTextureCount; i++)
		{
			if (mTextureArray.As<Texture2DArray>()->GetTexture(i) == texture)
				return i;
		}

		if (mTextureCount >= sMaxTextureCount)
			return -1;

		int32_t index = mTextureArray.As<Texture2DArray>()->Append(texture);
		if (index == -1)
			return -1;

		mTextureCount++;
		return index;
	}
} // namespace BHive