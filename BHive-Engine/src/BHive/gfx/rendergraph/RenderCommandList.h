#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Command.h"

namespace BHive
{
	class BufferBase;

	struct FBufferUse
	{
		BufferBase *Buffer;
		EBufferAccess Access;
	};

	struct FBufferBarrierRequest
	{
		BufferBase *Buffer;
		EBufferAccess Src;
		EBufferAccess Dst;
	};

	struct FRenderCommandList
	{
		std::vector<Ref<FCommand>> Commands;
		std::vector<FBufferBarrierRequest> BufferBarriers;

		template <typename T, typename... TArgs>
		T *Emplace(TArgs &&...args)
		{
			static_assert(std::is_base_of_v<FCommand, T>, "Type T doesn't derive from FCommand!");
			static_assert(std::is_constructible_v<T, TArgs...>, "Emplace<T>: Provided arguments doesn't match T's constrcutor");

			auto cmd = CreateRef<T>(std::forward<TArgs>(args)...);
			T *ptr = cmd.get();
			Commands.push_back(cmd);
			return ptr;
		}
	};
} // namespace BHive