#pragma once

#include "gfx/resources/ImageSubResourceRange.h"
#include "gfx/renderers/ViewSystem.h"
#include "GlobalBinding.h"
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
		std::vector<FPhase> Phases;
		FPassState State;
		std::unordered_map<GlobalBinding, Ref<BufferBase>> GlobalBuffers;
		std::unordered_map<GlobalBinding, Ref<Texture>> GlobalTextures;

		void BeginPhase(EPhaseType type = EPhaseType::Graphics);

		void BeginPhase(const std::string &name, EPhaseType type = EPhaseType::Graphics);

		template <typename T>
		T &Emplace()
		{
			ASSERT(mCurrentPhase);

			static_assert(std::is_base_of_v<FCommand, T>, "Type T doesn't derive from FCommand!");
			static_assert(std::is_constructible_v<T>, "Emplace<T>: Provided arguments doesn't match T's constrcutor");

			auto cmd = CreateRef<T>();
			mCurrentPhase->CommandList.Commands.emplace_back(cmd);
			return *cmd.get();
		}

		void Push(Ref<Framebuffer> fbo, ImageSubresourceRange colorRange = {});

		void Push(Ref<Texture> tex, EImageAccess access, ImageSubresourceRange range = {});

		void Push(Ref<BufferBase> buffer, EBufferAccess access);

		void PushGlobal(uint32_t set, uint32_t binding, const Ref<BufferBase> &buffer);

		void PushGlobal(uint32_t set, uint32_t binding, const Ref<Texture> &texture);

		void EndPhase();

	private:
		// global resources

		FPhase *mCurrentPhase = nullptr;
	};
} // namespace BHive