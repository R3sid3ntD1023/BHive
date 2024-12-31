#pragma once

#include "TAssetEditor.h"
#include "sprite/FlipBook.h"

namespace BHive
{

	class FlipBookEditor : public TAssetEditor<FlipBook>
	{
	protected:
		virtual void OnWindowRender();

		virtual const char *GetFileDialogFilter() { return "FlipBook (*.flipbook)\0*.flipbook\0"; };

	private:
		int mCurrentFrame = 0;
	};
}