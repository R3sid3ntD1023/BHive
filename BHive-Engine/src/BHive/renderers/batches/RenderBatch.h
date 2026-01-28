#pragma once

#include "core/Core.h"

namespace BHive
{
	class Shader;

	struct IRenderBatch
	{
		virtual ~IRenderBatch() = default;

		virtual void End() = 0;

		virtual void StartBatch() = 0;

		virtual void NextBatch() = 0;

		virtual void Flush() = 0;
	};
} // namespace BHive