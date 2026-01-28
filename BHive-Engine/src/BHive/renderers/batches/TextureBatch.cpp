#include "TextureBatch.h"
#include "gfx/Texture.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void TextureBatchData::Init()
	{
		mTextures[0] = Renderer::GetWhiteTexture();
	}

	void TextureBatchData::End()
	{
	}

	void TextureBatchData::StartBatch()
	{
		mTextureCount = 1;
	}

	void TextureBatchData::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void TextureBatchData::Flush()
	{
		for (size_t i = 0; i < mTextureCount; i++)
			mTextures[i]->Bind(i);
	}

	uint32_t TextureBatchData::GetTextureIndex(IRenderBatch &data, const Ref<Texture> &texture)
	{
		uint32_t texture_index = 0;
		for (uint32_t t = 0; t < mTextureCount; t++)
		{
			if (texture && mTextures[t] == texture)
			{
				texture_index = t;
				break;
			}
		}

		if (texture_index == 0 && texture)
		{
			if (mTextureCount >= sMaxTextureCount)
			{
				NextBatch();
				data.NextBatch();
			}

			texture_index = mTextureCount;
			mTextures[mTextureCount] = texture;
			mTextureCount++;
		}

		return texture_index;
	}
} // namespace BHive