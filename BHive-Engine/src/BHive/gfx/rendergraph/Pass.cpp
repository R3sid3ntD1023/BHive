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

	void FPass::UseFramebuffer(FramebufferPtr fbo, ImageSubresourceRange range)
	{
		ASSERT(mCurrentPhase)

		mCurrentPhase->BoundFBO = {fbo, range};

		auto fb = fbo.As<Framebuffer>();
		for (uint32_t i = 0; i < fb->GetNumColorAttachments(); i++)
		{
			UseTexture(fb->GetColorAttachment(i), EImageUsage::ColorWrite, range);
		}

		if (auto depth = fb->GetDepthAttachment())
		{
			UseTexture(depth, EImageUsage::DepthWrite, range);
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

	void FPass::BindResourceSet(ResourceSetPtr resourceSet)
	{
		ASSERT(mCurrentPhase)
		mCurrentPhase->ResourceSets.emplace_back(resourceSet);
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