#pragma once

#include "Command.h"
#include <rttr/variant.h>

namespace BHive
{
	struct FCommandProperty : public ICommand
	{
		FCommandProperty(rttr::variant obj, const rttr::property &prop, const rttr::variant &new_value);

		virtual void on_undo() override;

		virtual void on_redo() override;

		virtual bool can_merge() const { return true; }

		virtual bool merge(ICommand *other) override;

	private:
		rttr::variant mValue;
		rttr::variant mOldValue;

		rttr::property mProperty;
		rttr::variant mInstance;
	};

} // namespace BHive