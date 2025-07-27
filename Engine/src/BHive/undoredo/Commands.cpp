#include "Commands.h"

namespace BHive
{
	FCommandProperty::FCommandProperty(rttr::instance obj, const rttr::property &prop, const rttr::variant &new_value)
		: mProperty(prop),
		  mInstance(obj),
		  mValue(new_value)
	{
		mOldValue = mProperty.get_value(mInstance);
	}

	void FCommandProperty::OnUndo()
	{
		mProperty.set_value(mInstance, mOldValue);
	}

	void FCommandProperty::OnRedo()
	{
		mProperty.set_value(mInstance, mValue);
	}
} // namespace BHive