#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"
#include <ImNodeFlow.h>

namespace BHive
{
	class AnimationGraph : public ImFlow::ImNodeFlow
	{
	public:
		AnimationGraph();

		virtual void DrawCreateNodeMenu(const ImVec2 &pos);

		AnimationGraph *GetCurrentGraph() const { return mCurrentGraph; }

	protected:
		virtual rttr::type GetSupportedNodeType() const;

	private:
		std::vector<rttr::type> mDerivedNodeTypes;
		AnimationGraph *mCurrentGraph = nullptr;
	};
} // namespace BHive