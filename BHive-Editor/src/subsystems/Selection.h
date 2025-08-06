#pragma once

namespace BHive
{
	class GameObject;

	struct Selection
	{

		void Select(GameObject *obj);

		void Clear();

		GameObject *GetSelection() const;

		bool IsSelected(GameObject *obj) const;

	private:
		GameObject *mCurrentSelection = nullptr;
	};

} // namespace BHive
