#pragma once

#include "TAssetEditor.h"
#include "sprite/FlipBook.h"

namespace BHive
{

	class FlipBookEditor : public TAssetEditor<FlipBook>
	{
	protected:
		virtual void OnUpdateContent() override;

	private:
		int mCurrentFrame = 0;
	};
} // namespace BHive