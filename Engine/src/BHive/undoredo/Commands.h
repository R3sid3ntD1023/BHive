#pragma once

#include "Command.h"
#include <rttr/variant.h>

namespace BHive
{
	struct FCommandProperty : public ICommand
	{
		FCommandProperty(rttr::instance obj, const rttr::property &prop, const rttr::variant &new_value);

		virtual void OnUndo();

		virtual void OnRedo();

	private:
		rttr::variant mValue;
		rttr::variant mOldValue;

		rttr::property mProperty;
		rttr::instance mInstance;
	};

} // namespace BHive