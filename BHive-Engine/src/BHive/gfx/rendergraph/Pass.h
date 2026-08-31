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

	struct BHIVE_API FPass
	{
		std::string Name;
		EPassType Type{};
		std::vector<FPhase> Phases;
		FPassState State;

		void BeginPhase(EPhaseType type = EPhaseType::Graphics);

		void BeginPhase(const std::string &name, EPhaseType type = EPhaseType::Graphics);

		template <typename T>
		T &Emplace()
		{
			ASSERT(mCurrentPhase);

			static_assert(std::is_base_of_v<FCommand, T>, "Type T doesn't derive from FCommand!");

			CmdHeader header{T::Type, sizeof(T)};
			auto cmd = CreateRef<T>();
			mCurrentPhase->Commands.emplace_back(header, cmd);
			return *cmd.get();
		}

		// template <typename T>
		// T &Emplace()
		// {
		// 	ASSERT(mCurrentPhase);

		// 	T cmd{};

		// 	CmdHeader header{T::Type, sizeof(T)};

		// 	auto &commands = mCurrentPhase->CommandPtr;
		// 	size_t start = commands.size();
		// 	commands.resize(start + sizeof(CmdHeader) + sizeof(T));
		// 	auto data = commands.data();

		// 	std::memcpy(data + start, &header, sizeof(CmdHeader));
		// 	std::memcpy(data + start + sizeof(CmdHeader), &cmd, sizeof(T));

		// 	T *ptr = reinterpret_cast<T *>(data + start + sizeof(CmdHeader));
		// 	ASSERT(ptr);
		// 	return *ptr;
		// }

		void UseFramebuffer(FramebufferPtr fbo, ImageSubresourceRange colorRange = {});

		void UseTexture(TexturePtr tex, EImageUsage access, ImageSubresourceRange range = {});

		void UseBuffer(Ref<BufferBase> buffer, EBufferUsage access);

		void BindGlobal(uint32_t set, uint32_t binding, const Ref<BufferBase> &buffer);

		void BindGlobal(uint32_t set, uint32_t binding, TexturePtr texture);

		void EndPhase();

		void ResolveBufferTransitons();

	private:
		// global resources

		FPhase *mCurrentPhase = nullptr;
	};
} // namespace BHive