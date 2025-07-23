#pragma once

#include "core/Core.h"
#include <ImNodeFlow.h>

namespace BHive
{
	class AnimGraph;

	class AnimationGraph : public ImFlow::ImNodeFlow
	{
	public:
		AnimationGraph(const Ref<AnimGraph> &graph);

		virtual void DrawCreateNodeMenu(const ImVec2 &pos);

	protected:
		virtual rttr::type GetSupportedNodeType() const;

	private:
		std::vector<rttr::type> mDerivedNodeTypes;
		Ref<AnimGraph> mGraph;
	};
} // namespace BHive