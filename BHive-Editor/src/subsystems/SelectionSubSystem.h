#pragma once

namespace BHive
{

	struct GameObject;

	struct SelectionSubSystem
	{
		void Select(GameObject *obj) { mCurrentSelection = obj; }
		void Clear() { mCurrentSelection = nullptr; }

		GameObject *GetSelection() const { return mCurrentSelection; }

	private:
		GameObject *mCurrentSelection = nullptr;
	};

} // namespace BHive
