#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "Command.h"

namespace BHive
{
	class BufferBase;

	struct FBufferUsageInfo
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Access;
	};

	struct FBufferTransition
	{
		Ref<BufferBase> Buffer;
		EBufferUsage Src;
		EBufferUsage Dst;
	};

	struct FRenderCommandList
	{
		std::vector<Ref<FCommand>> Commands;
		std::vector<FBufferTransition> BufferBarriers;
	};
} // namespace BHive