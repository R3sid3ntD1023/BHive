#pragma once

#include "core/Buffer.h"
#include "gfx/renderers/ViewSystem.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include "Phase.h"

namespace BHive
{

	enum class EPassType : uint8_t
	{
		OffScreen,
		Present
	};

	enum class EAttachmentLoadState
	{
		None,
		DontCare,
		Clear,
		Load
	};

	enum class EAttachmentStoreState
	{
		None,
		DontCare,
		Store
	};

	struct FAttachmentState
	{
		EAttachmentLoadState LoadOP = EAttachmentLoadState::Clear;
		EAttachmentStoreState StoreOP = EAttachmentStoreState::Store;
		glm::vec4 ClearColor{0, 0, 0, 1};
	};

	struct FPassState
	{
		FAttachmentState Color;
		FAttachmentState Depth;
	};

	struct BHIVE_API FPass
	{
		std::string Name;
		EPassType Type{};
		FPassState State;
		std::list<FPhase> Phases;

		void BeginPhase(EPhaseType type, const std::string &name = "");

		template <typename T>
		T &Emplace()
		{
			ASSERT(mCurrentPhase);
			return mCurrentPhase->Commands->Emplace<T>(T::Type);
		}

		void
		UseFramebuffer(FramebufferPtr fbo, ImageSubresourceRange range = {}, EImageUsage colorUsage = EImageUsage::ColorWrite, EImageUsage depthUsage = EImageUsage::DepthWrite);

		void UseTexture(Ref<Texture> tex, EImageUsage access, ImageSubresourceRange range = {});

		void UseBuffer(BufferPtr buffer, EBufferUsage access);

		void BindResourceSet(ResourceSetPtr resourceSet);

		void EndPhase();

		void ResolveBufferTransitons();

	private:
		// global resources

		FPhase *mCurrentPhase = nullptr;
	};
} // namespace BHive