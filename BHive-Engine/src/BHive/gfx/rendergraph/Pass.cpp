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

	void FPass::UseFramebuffer(FramebufferPtr fbo, ImageSubresourceRange colorRange)
	{
		ASSERT(mCurrentPhase)

		mCurrentPhase->FBO = fbo;
		mCurrentPhase->ColorRange = colorRange;

		auto fb = fbo.As<Framebuffer>();
		for (uint32_t i = 0; i < fb->GetNumColorAttachments(); i++)
		{
			UseTexture(fb->GetColorAttachment(i), EImageUsage::ColorWrite, colorRange);
		}

		if (auto depth = fb->GetDepthAttachment())
		{
			UseTexture(depth, EImageUsage::DepthWrite);
		}
	}

	void FPass::UseTexture(TexturePtr tex, EImageUsage access, ImageSubresourceRange range)
	{
		ASSERT(mCurrentPhase)

		mCurrentPhase->Images.emplace_back(tex, access, range);
	}

	void FPass::UseBuffer(BufferPtr buffer, EBufferUsage access)
	{
		ASSERT(mCurrentPhase)
		mCurrentPhase->Buffers.push_back({buffer, access});
	}

	void FPass::BindBuffer(uint32_t set, uint32_t binding, BufferPtr buffer)
	{
		ASSERT(mCurrentPhase && buffer)
		mCurrentPhase->BoundBuffers[GlobalBinding{set, binding}] = buffer;
	}

	void FPass::BindTexture(uint32_t set, uint32_t binding, TexturePtr texture)
	{
		ASSERT(mCurrentPhase && texture)
		mCurrentPhase->BoundTextures[GlobalBinding{set, binding}] = texture;
	}

	void FPass::EndPhase()
	{
		ASSERT(mCurrentPhase)
		mCurrentPhase = nullptr;
	}

	void FPass::ResolveBufferTransitons()
	{
		std::unordered_map<BufferPtr, EBufferUsage> lastBufferAccess;

		for (auto &phase : Phases)
		{
			// buffers
			for (auto &use : phase.Buffers)
			{
				auto buffer = use.Buffer;
				auto prev = lastBufferAccess[buffer];
				auto next = use.Access;

				if (prev != next)
					phase.BufferTransitions.emplace_back(FBufferTransition{buffer, prev, next});

				lastBufferAccess[buffer] = next;
			}
		}
	}
} // namespace BHive