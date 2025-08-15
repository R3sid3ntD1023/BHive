#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ICommand
	{
		virtual ~ICommand() = default;

		virtual void OnUndo() = 0;

		virtual void OnRedo() = 0;

		virtual bool CanMerge() const { return false; }

		virtual bool Merge(const std::shared_ptr<ICommand> &other) { return false; }
	};
} // namespace BHive