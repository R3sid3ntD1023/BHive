#include "inspector/Inspector.h"

namespace BHive
{
	void Inspector::BeginInspect(const rttr::property &property, bool columns)
	{
		mLayout = PropertyLayout(property, columns);
		mLayout.PushLayout();
	}

	void Inspector::EndInspect(const rttr::property &property)
	{
		mLayout.PopLayout();
	}
} // namespace BHive