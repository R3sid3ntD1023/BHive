#include "Inspector.h"

namespace BHive
{
	void Inspector::begin_inspect(const rttr::property &property, bool columns, float width)
	{
		mLayout = PropertyLayout(property, columns, width);
		mLayout.PushLayout();
	}

	void Inspector::end_inspect(const rttr::property &property)
	{
		mLayout.PopLayout();
	}
} // namespace BHive