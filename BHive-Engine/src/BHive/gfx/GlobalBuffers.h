#pragma once

#include "core/Core.h"
#include "gfx/NativeHandle.h"

namespace BHive
{
	class BufferBase;

	struct GlobalBuffers
	{
		struct BufferHandle
		{
			Ref<BufferBase> Buffer;
			const NativeHandle GetHandle(uint32_t frame) const;
		};

		void Register(uint32_t binding, const BufferHandle &buffer);

		const BufferHandle GetBuffer(uint32_t binding) const { return mBuffers.at(binding); }

		bool Contains(uint32_t binding) const;

	private:
		std::unordered_map<uint32_t, BufferHandle> mBuffers;
	};

}