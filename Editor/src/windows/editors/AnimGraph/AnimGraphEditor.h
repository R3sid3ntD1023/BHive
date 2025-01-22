#pragma once

#include "Animator/anim_graph/AnimGraph.h"
#include "graph/Graph.h"
#include "windows/editors/TAssetEditor.h"
#include <ImNodeFlow.h>

namespace BHive
{
	class AnimGraphEditor : public TAssetEditor<AnimGraph>
	{
	public:
		AnimGraphEditor();

		virtual void OnWindowRender() override;

	private:
		void DrawNodeList();
		void DrawEditorWindow();
		void DrawCreateNodeMenu(const ImVec2 &pos);

		std::vector<rttr::type> mDerivedNodes;
		Graph mGraph;
	};

} // namespace BHive