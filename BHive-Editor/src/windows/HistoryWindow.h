#pragma once

#include "WindowBase.h"

namespace BHive
{
	class HistoryWindow : public WindowBase
	{
	public:
		HistoryWindow() = default;

	protected:
		void OnGuiRender() override;

		virtual const char *GetName() const { return "History"; }
	};
} // namespace BHive