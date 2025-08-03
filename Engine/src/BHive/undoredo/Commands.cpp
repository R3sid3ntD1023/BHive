#include "Commands.h"

namespace BHive
{
	FCommandProperty::FCommandProperty(rttr::variant obj, const rttr::property &prop, const rttr::variant &new_value)
		: mProperty(prop),
		  mInstance(obj),
		  mValue(new_value)
	{
		mOldValue = mProperty.get_value(mInstance);
	}

	void FCommandProperty::on_undo()
	{
		mProperty.set_value(mInstance, mOldValue);
	}

	void FCommandProperty::on_redo()
	{
		mProperty.set_value(mInstance, mValue);
	}

	bool FCommandProperty::merge(ICommand *other)
	{
		FCommandProperty *command = dynamic_cast<FCommandProperty *>(other);
		if (command)
		{
			if (command->mInstance == mInstance && mOldValue == command->mOldValue)
			{
				command->mValue = mValue;
				return true;
			}

			return false;
		}
	}
} // namespace BHive