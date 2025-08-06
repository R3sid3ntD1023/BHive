#include "Selection.h"

namespace BHive
{

	void Selection::Select(GameObject *obj)
	{
		if (mCurrentSelection != obj)
			mCurrentSelection = obj;
	}

	void Selection::Clear()
	{
		mCurrentSelection = nullptr;
	}

	GameObject *Selection::GetSelection() const
	{
		return mCurrentSelection;
	}

	bool Selection::IsSelected(GameObject *obj) const
	{
		return mCurrentSelection == obj;
	}

} // namespace BHive