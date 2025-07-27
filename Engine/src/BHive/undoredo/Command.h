#pragma once

#include "core/Core.h"

namespace BHive
{
	struct FHistoryCommand
	{
		virtual void OnUndo() = 0;

		virtual void OnRedo() = 0;
	};
} // namespace BHive