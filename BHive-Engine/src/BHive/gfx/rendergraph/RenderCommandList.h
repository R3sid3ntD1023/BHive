#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Command.h"

namespace BHive
{
	class BufferBase;

	struct FBufferUse
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Access;
	};

	struct FBufferBarrierRequest
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Src;
		EBufferUsage Dst;
	};

	struct FRenderCommandList
	{
		std::vector<Ref<FCommand>> Commands;
		std::vector<FBufferBarrierRequest> BufferBarriers;
	};
} // namespace BHive