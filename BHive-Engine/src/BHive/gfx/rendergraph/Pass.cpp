#include "Pass.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	
	void FPass::BeginPhase(EPhaseType type)
	{
		auto name = std::format("Phase{}", mCurrentPhase);
		BeginPhase(name, type);
	}

	void FPass::BeginPhase(const std::string &name, EPhaseType type)
	{
		Phases.emplace_back(name, type);
		mCurrentPhase = (int32_t)Phases.size() - 1;
	}

	void FPass::Push(const FView &view)
	{
		View = view;
	}

	void FPass::Push(Ref<Framebuffer> fbo)
	{
		ASSERT(mCurrentPhase > -1)
		Phases[mCurrentPhase].FBO = fbo;

		for (uint32_t i = 0; i < fbo->GetNumColorAttachments(); i++)
			Push(fbo->GetColorAttachment(i), EImageAccess::ColorWrite);

		if (auto depth = fbo->GetDepthAttachment())
			Push(depth, EImageAccess::DepthWrite);
	}

	void FPass::Push(Ref<Texture> tex, EImageAccess access, ImageSubresourceRange range)
	{
		ASSERT(mCurrentPhase > -1)

		if(range.LevelCount == 1 && range.LayerCount == 1)
		{
			range.LevelCount = tex->GetInfo().MipLevels;
			range.LayerCount = tex->GetInfo().ArrayLayers;
		}

		Phases[mCurrentPhase].Images.emplace_back(tex, access, range);
	}

	void FPass::Push(BufferBase* buffer, EBufferAccess access)
	{
		ASSERT(mCurrentPhase > -1)
		Phases[mCurrentPhase].Buffers.push_back({buffer, access});
	}

	void FPass::EndPhase()
	{
		ASSERT(mCurrentPhase > -1)
		mCurrentPhase = -1;
	}
}