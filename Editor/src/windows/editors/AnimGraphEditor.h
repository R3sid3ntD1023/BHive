#pragma once

#include "Animator/anim_graph/AnimGraph.h"
#include "TAssetEditor.h"

namespace BHive
{
	class AnimGraphEditor : public TAssetEditor<AnimGraph>
	{
	public:
		virtual void OnWindowRender() override;

	private:
		void DrawNodeList();
		void DrawEditorWindow();
	};

} // namespace BHive