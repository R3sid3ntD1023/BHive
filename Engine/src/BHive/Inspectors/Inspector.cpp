#include "Inspector.h"

namespace BHive
{
	void Inspector::BeginInspect(const rttr::property &property, bool columns, float width)
	{
		mLayout = PropertyLayout(property, columns, width);
		mLayout.PushLayout();
	}

	void Inspector::EndInspect(const rttr::property &property)
	{
		mLayout.PopLayout();
	}
} // namespace BHive