#pragma once

#include "ImWindowBase.h"

namespace BHive
{
	class ImHistoryWindow : public ImWindowBase
	{
	public:
		ImHistoryWindow();

	protected:
		void OnUpdateContent() override;

		virtual const char *GetName() const override { return "History"; }
	};
} // namespace BHive