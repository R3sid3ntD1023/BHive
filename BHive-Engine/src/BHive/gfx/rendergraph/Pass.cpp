#include "Pass.h"
#include "gfx/Framebuffer.h"

namespace BHive
{

	void FPass::BeginPhase(EPhaseType type)
	{
		auto name = std::format("Phase{}", (uint32_t)Phases.size());
		BeginPhase(name, type);
	}

	void FPass::BeginPhase(const std::string &name, EPhaseType type)
	{
		Phases.emplace_back(name, type);
		mCurrentPhase = &Phases.back();
	}

	void FPass::UseFramebuffer(Ref<Framebuffer> fbo, ImageSubresourceRange colorRange)
	{
		ASSERT(mCurrentPhase)

		mCurrentPhase->FBO = fbo;
		mCurrentPhase->ColorRange = colorRange;

		for (uint32_t i = 0; i < fbo->GetNumColorAttachments(); i++)
		{
			UseTexture(fbo->GetColorAttachment(i), EImageUsage::ColorWrite, colorRange);
		}

		if (auto depth = fbo->GetDepthAttachment())
		{
			UseTexture(depth, EImageUsage::DepthWrite);
		}
	}

	void FPass::UseTexture(Ref<Texture> tex, EImageUsage access, ImageSubresourceRange range)
	{
		ASSERT(mCurrentPhase)

		mCurrentPhase->Images.emplace_back(tex, access, range);
	}

	void FPass::UseBuffer(Ref<BufferBase> buffer, EBufferUsage access)
	{
		ASSERT(mCurrentPhase)
		mCurrentPhase->Buffers.push_back({buffer, access});
	}

	void FPass::BindGlobal(uint32_t set, uint32_t binding, const Ref<BufferBase> &buffer)
	{
		ASSERT(buffer)
		GlobalBuffers[GlobalBinding{set, binding}] = buffer;
	}

	void FPass::BindGlobal(uint32_t set, uint32_t binding, const Ref<Texture> &texture)
	{
		ASSERT(texture)
		GlobalTextures[GlobalBinding{set, binding}] = texture;
	}

	void FPass::EndPhase()
	{
		ASSERT(mCurrentPhase)
		mCurrentPhase = nullptr;
	}
} // namespace BHive