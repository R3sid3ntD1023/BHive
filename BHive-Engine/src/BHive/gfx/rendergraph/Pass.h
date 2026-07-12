#pragma once

#include "gfx/resources/ImageSubResourceRange.h"
#include "gfx/renderers/ViewSystem.h"
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

	template <typename T>
	struct CommandBuilder
	{
		FRenderCommandList &List;

		CommandBuilder(FRenderCommandList &list)
			: List(list)
		{
		}

		template <typename... TArgs>
		T *operator()(TArgs &&...args)
		{
			return List.Emplace<T>(std::forward<TArgs>(args)...);
		}
	};

	struct BHIVE_API FPass
	{
		std::string Name;
		EPassType Type{};
		std::vector<FPhase> Phases;
		std::optional<FView> View;
		FPassState State;

		void BeginPhase(EPhaseType type = EPhaseType::Graphics);

		void BeginPhase(const std::string &name, EPhaseType type = EPhaseType::Graphics);

		template <typename T>
		CommandBuilder<T> Emplace()
		{
			ASSERT(mCurrentPhase > -1);
			return CommandBuilder<T>(Phases[mCurrentPhase].CommandList);
		}

		void Push(const FView &view);

		void Push(Ref<Framebuffer> fbo, ImageSubresourceRange colorRange = {});

		void Push(Ref<Texture> tex, EImageAccess access, ImageSubresourceRange range = {});

		void Push(BufferBase *buffer, EBufferAccess access);

		void EndPhase();

		const FView &GetView() const { return View.value(); }

		bool HasView() const { return View.has_value(); }

	private:
		int32_t mCurrentPhase = -1;
	};
} // namespace BHive