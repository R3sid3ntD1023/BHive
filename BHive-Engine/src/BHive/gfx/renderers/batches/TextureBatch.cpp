#include "TextureBatch.h"
#include "gfx/Texture.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{
	void TextureBatchData::Init()
	{
		FTextureCreateInfo create_info{};
		create_info.Format = EFormat::RGBA8;
		create_info.Aspect = ETextureAspect::Color;
		create_info.Usage = ETextureUsage::Sampled | ETextureUsage::TransferDst;
		create_info.ArrayLayers = sMaxTextureCount;

		mTextureArray = Texture2DArray::Create({512, 512}, create_info);
		auto white =  Cast<Texture2D>(Renderer::GetWhiteTexture());

		mTextureArray->AddTexture(white);
		mTextureArray->SetStartLayer(1);
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
		
	}

	int32_t TextureBatchData::GetTextureIndex(IRenderBatch &data, const Ref<Texture> &texture)
	{
		auto tex = Cast<Texture2D>(texture);
		auto index = mTextureArray->AddTexture(tex);
		
		if (index == -1)
		{
			data.Flush();	
			mTextureArray->Clear();
			index = mTextureArray->AddTexture(tex);
		}
		return index;
	}
} // namespace BHive