#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ICommand
	{
		virtual void on_undo() = 0;

		virtual void on_redo() = 0;

		virtual bool can_merge() const { return false; }

		virtual bool merge(ICommand *other) { return false; }
	};
} // namespace BHive