#pragma once

#include "Command.h"
#include "core/BufferArena.h"
#include "core/Core.h"
#include "gfx/Enumerations.h";
#include "gfx/registries/Handles.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include <cstddef>
#include <new>

namespace BHive
{
	class Framebuffer;
	class BufferBase;

	enum class EPhaseType
	{
		Graphics,
		Compute,
		Transfer
	};

	// Tex, Mip, Levels, Layer, Layers, Access
	struct FImageInfo
	{
		TexturePtr Texture;
		EImageUsage Access = EImageUsage::None;
		ImageSubresourceRange Range{};
	};

	struct FBufferUsageInfo
	{
		BufferPtr Buffer;
		EBufferUsage Access;
	};

	struct FBufferTransition
	{
		BufferPtr Buffer;
		EBufferUsage Src;
		EBufferUsage Dst;
	};

	struct CmdHeader
	{
		ECommandType Type;
		size_t Size = 0;
		size_t PayloadOffset = 0;
		size_t RecordSize = 0;
	};

	class FCommandStream
	{
	public:
		template <typename T>
		T &Emplace(ECommandType type)
		{
			static_assert(std::is_trivially_copyable_v<T>);
			static_assert(std::is_trivially_destructible_v<T>);
			static_assert(std::is_standard_layout_v<T>);
			static_assert(alignof(T) <= alignof(std::max_align_t));

			const size_t payloadOffset = Align(sizeof(CmdHeader), alignof(T));
			const size_t recordSize = Align(payloadOffset + sizeof(T), alignof(CmdHeader));
			auto *record = mStorage.Allocate(recordSize, alignof(CmdHeader));
			::new (record) CmdHeader{type, sizeof(T), payloadOffset, recordSize};
			auto *command = ::new (record + payloadOffset) T{};
			if constexpr (requires(T &value) { value.Storage; })
				command->Storage = this;

			return *command;
		}

		template <typename T, typename... TArgs>
		uint32_t StoreObject(TArgs &&...args)
		{
			ASSERT(mObjects.size() < UINT32_MAX, "Command sidecar index overflow");
			auto object = CreateRef<T>(std::forward<TArgs>(args)...);
			mObjects.emplace_back(object);
			return static_cast<uint32_t>(mObjects.size() - 1);
		}

		size_t StoreBytes(const void *data, size_t size)
		{
			const size_t offset = mData.GetUsedSize();
			if (size == 0)
				return offset;

			auto *destination = mData.Allocate(size, alignof(std::max_align_t));
			std::memcpy(destination, data, size);
			return static_cast<size_t>(destination - mData.GetBuffer().GetData());
		}

		const std::byte *GetData(size_t offset) const { return mData.GetBuffer().GetData() + offset; }

		template <typename T>
		T &GetObject(uint32_t index)
		{
			ASSERT(index < mObjects.size(), "Invalid command sidecar index");
			return *static_cast<T *>(mObjects[index].get());
		}

		const std::byte *Data() const { return mStorage.GetBuffer().GetData(); }

		size_t GetUsedSize() const { return mStorage.GetUsedSize(); }

	private:
		static size_t Align(size_t value, size_t alignment) { return (value + alignment - 1) & ~(alignment - 1); }

		BufferArena<std::byte> mStorage{64 * 1024};
		BufferArena<std::byte> mData{64 * 1024};
		std::vector<Ref<void>> mObjects;
	};

	struct BoundFramebuffer
	{
		FramebufferPtr FBO;

		ImageSubresourceRange Range;
	};

	struct FPhase
	{
		std::string Name;

		EPhaseType Type = EPhaseType::Graphics;

		BoundFramebuffer BoundFBO;

		std::vector<FImageInfo> Images;

		std::vector<FBufferUsageInfo> Buffers;

		std::vector<FBufferTransition> BufferTransitions;

		std::vector<ResourceSetPtr> ResourceSets;

		Ref<FCommandStream> Commands = CreateRef<FCommandStream>();
	};

} // namespace BHive
